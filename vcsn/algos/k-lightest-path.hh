#pragma once

#include <algorithm>

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/weightset/weightset.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/algos/filter.hh>

namespace vcsn
{
  /// Yen implementation.
  ///
  /// Retrieve the K lightest path in an automaton.
  struct yen_tag {};

  namespace detail
  {
    /// Yen implementation of the K lightest automaton algorithm.
    ///
    /// Functor initialized by the automaton on which the lightest paths will
    /// be computed. And called with the source and destination states of the
    /// path, as long as the number (k) of paths to retrieve.
    ///
    /// \tparam ValueSet could be either a labelset or weightset.
    ///         Must have a less and a one member function.
    /// \tparam Mul lambda multiplying the current best candidate with
    ///         the value taken from the transition given in parameter.
    /// \tparam GetValue lambda used to retrieve the value_type expected by the
    ///         single lightest path algorithm from a monomial.
    template <Automaton Aut, typename ValueSet, typename Mul, typename GetValue>
    struct yen_impl
    {
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using path_t = std::vector<transition_t>;
      using paths_t = std::vector<path_t>;

      using valueset_t = ValueSet;
      using value_t = typename valueset_t::value_t;

      yen_impl(const automaton_t& aut, const ValueSet& vs, Mul mul, GetValue get_value)
        : aut_{aut}
        , vs_{vs}
        , mul_{mul}
        , get_value_(get_value)
      {}

      struct profile
      {
        profile(const path_t& path, const value_t& v, const valueset_t& vs)
          : path_{path}
          , v_{v}
          , vs_{vs}
        {}

        bool operator<(const profile& rhs) const
        {
          return vs_.less(rhs.v_, v_);
        }

        path_t path_;
        value_t v_;
        const valueset_t& vs_;
      };

      using heap_t = boost::heap::fibonacci_heap<profile>;

      template <Automaton AnyAut>
      path_t
      path(const AnyAut& aut,
           const path_t& path,
           state_t_of<Aut> src = Aut::element_type::pre(),
           state_t_of<Aut> dst = Aut::element_type::post())
      {
        path_t res;
        for (auto t = path[dst];
             t != aut->null_transition();
             t = path[aut->src_of(t)])
          {
            res.emplace_back(t);
            if (aut->src_of(t) == src)
              break;
          }
        std::reverse(res.begin(), res.end());
        return res;
      }

      template <Automaton AnyAut>
      path_t
      compute_lightest_path(const AnyAut& aut,
                            state_t_of<Aut> src = Aut::element_type::pre(),
                            state_t_of<Aut> dst = Aut::element_type::post())
      {
        auto algo = detail::make_dijkstra_impl(aut, vs_, mul_);
        return std::move(algo(src, dst));
      }

      paths_t
      operator()(state_t src, state_t dst, unsigned k)
      {
        auto first = compute_lightest_path(aut_, src, dst);
        auto res = paths_t{path(aut_, first, src, dst)};
        auto ps = make_word_polynomialset(aut_->context());

        auto heap = heap_t();

        for (unsigned i = 1u; i < k; i++)
          {
            const auto& prev = res[i - 1];
            for (unsigned j = 0u; j < prev.size(); j++)
              {
                auto filter_aut = filter<automaton_t, true>(aut_);
                filter_aut->unhide_all_states();
                filter_aut->unhide_all_transition();
                auto spur_node = filter_aut->src_of(prev[j]);
                auto root_path = path_t(prev.begin(), prev.begin() + j);

                for (const auto& selected_path: res)
                  if (j < selected_path.size())
                    {
                      auto diff = std::mismatch(root_path.begin(), root_path.end(),
                                                selected_path.begin(), selected_path.begin() + j);
                      if (diff.first == root_path.end()
                          && filter_aut->has_transition(selected_path[j]))
                        filter_aut->hide_transition(selected_path[j]);
                    }

                for (auto t: root_path)
                  if (t != filter_aut->null_transition()
                      && filter_aut->src_of(t) != spur_node
                      && filter_aut->src_of(t) != aut_->pre()
                      && filter_aut->has_state(filter_aut->src_of(t)))
                    filter_aut->hide_state(filter_aut->src_of(t));

                auto shortest_path = compute_lightest_path(filter_aut, spur_node, dst);
                auto spur_path = path(filter_aut, shortest_path, spur_node, dst);
                root_path.insert(root_path.end(), spur_path.begin(), spur_path.end());
                if (!root_path.empty()
                    && filter_aut->src_of(root_path.front()) == src
                    && filter_aut->dst_of(root_path.back()) == dst)
                  {
                    bool already_found = false;
                    for (const auto& profile: heap)
                      {
                        auto& selected_path = profile.path_;
                        if (root_path.size() == selected_path.size())
                          {
                            auto diff = std::mismatch(root_path.begin(), root_path.end(),
                                                      selected_path.begin(), selected_path.end());
                            if (diff.first == root_path.end())
                            {
                              already_found = true;
                              break;
                            }
                          }
                      }
                    if (!already_found)
                      {
                        auto m = *path_monomial(filter_aut, format_lightest(filter_aut, root_path), src, dst);
                        heap.emplace(std::move(root_path), get_value_(m), vs_);
                      }
                  }
              }
            if (heap.empty())
              break;
            res.push_back(heap.top().path_);
            heap.pop();
          }
        return res;
      }

      const automaton_t& aut_;
      const ValueSet& vs_;
      Mul mul_;
      GetValue get_value_;
    };

    template <Automaton Aut, typename ValueSet, typename Mul, typename GetValue>
    auto
    make_yen(const Aut& aut, const ValueSet& vs, Mul mul, GetValue get_value)
    {
      return detail::yen_impl<Aut, ValueSet, Mul, GetValue>(aut, vs, mul, get_value);
    }
  }

  template <Automaton Aut>
  std::vector<std::vector<transition_t_of<Aut>>>
  k_lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest, unsigned k)
  {
    auto mul = [&aut](auto lhs, transition_t_of<Aut> t)
               {
                 return aut->weightset()->mul(lhs, aut->weight_of(t));
               };
    auto get_value = [](auto m) { return m.second; };
    auto yen = detail::make_yen(aut, *aut->weightset(), mul, get_value);
    return yen(source, dest, k);
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  format_lightest(const Aut& aut, const std::vector<transition_t_of<Aut>>& path)
  {
    auto res = std::vector<transition_t_of<Aut>>(states_size(aut),
                                                 aut->null_transition());
    for (auto t : path)
      if (t != aut->null_transition())
        res[aut->dst_of(t)] = t;
    return res;
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest,
                yen_tag)
  {
    auto paths = k_lightest_path(aut, source, dest, 1);
    auto res = paths.empty() ? std::vector<transition_t_of<Aut>>() : paths.front();
    return format_lightest(aut, res);
  }
}
