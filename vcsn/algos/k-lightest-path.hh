#pragma once

#include <algorithm>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/range/algorithm/mismatch.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/filter.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/misc/fibonacci_heap.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  /// Yen implementation.
  ///
  /// Retrieve the K lightest paths in an automaton.
  struct yen_tag {};

  namespace detail
  {
    /// Yen's algorithm of the K lightest paths.
    ///
    /// Functor initialized by the automaton on which the lightest paths will
    /// be computed. And called with the source and destination states of the
    /// path, as long as the number (k) of paths to retrieve.
    ///
    /// \tparam Aut
    ///   The type of the automaton.
    /// \tparam ValueSet
    ///   could be either a labelset or weightset.
    ///   Must have a less and a one member function.
    /// \tparam Mul
    ///   lambda multiplying the current best candidate with
    ///   the value taken from the transition given in parameter.
    /// \tparam GetValue
    ///   lambda used to retrieve the value_type expected by the
    ///   single lightest path algorithm from a monomial.
    template <Automaton Aut, typename ValueSet,
              typename Mul, typename GetValue>
    struct yen_impl
    {
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using path_t = path_t_of<automaton_t>;
      using paths_t = std::vector<path_t>;

      using valueset_t = ValueSet;
      using value_t = typename valueset_t::value_t;

      yen_impl(const automaton_t& aut, const ValueSet& vs,
               Mul mul, GetValue get_value)
        : aut_{aut}
        , vs_{vs}
        , mul_{mul}
        , get_value_(get_value)
      {}

      struct profile
      {
        profile(const path_t& path, const valueset_t& vs,
                const value_t& v)
          : path_{path}
          , vs_{vs}
          , v_{v}
        {}

        bool operator<(const profile& rhs) const
        {
          return vs_.less(v_, rhs.v_);
        }

        path_t path_;
        const valueset_t& vs_;
        value_t v_;
      };

      /// Min heap according to the weight.
      using heap_t = vcsn::min_fibonacci_heap<profile>;

      /// Transform a map transition_t -> transition_t representing the
      /// predecessors of each transition into a list of transitions used
      /// for concatenating paths.
      ///
      /// \tparam AnyAut
      ///   the automaton parameter needs to be generic as this function could
      ///   be used with either automaton_t or the filter automaton version
      ///   of automaton_t.
      template <Automaton AnyAut>
      path_t_of<AnyAut>
      path(const AnyAut& aut,
           const predecessors_t_of<AnyAut>& path,
           state_t_of<AnyAut> src = AnyAut::element_type::pre(),
           state_t_of<AnyAut> dst = AnyAut::element_type::post())
      {
        auto res = path_t_of<AnyAut>{};
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

      /// Compute a lightest path on a part of the automaton.
      ///
      /// \tparam AnyAut
      ///   the automaton parameter needs to be generic as this function could
      ///   be used with either automaton_t or the filter automaton version
      ///   of automaton_t.
      template <Automaton AnyAut>
      predecessors_t_of<AnyAut>
      compute_lightest_path(const AnyAut& aut,
                            state_t_of<AnyAut> src = Aut::element_type::pre(),
                            state_t_of<AnyAut> dst = Aut::element_type::post())
      {
        auto algo = detail::make_dijkstra_impl(aut, vs_, mul_);
        return algo(src, dst);
      }

      paths_t
      operator()(state_t src, state_t dst, unsigned k)
      {
        using boost::starts_with;
        using boost::make_iterator_range;
        auto first = compute_lightest_path(aut_, src, dst);
        auto res = paths_t{path(aut_, first, src, dst)};
        auto ps = make_word_polynomialset(aut_->context());

        auto heap = heap_t{};

        for (auto i = 1u; i < k; i++)
          {
            const auto& prev = res[i - 1];
            for (auto j = 0u; j < prev.size(); j++)
              {
                auto filter_aut = filter<automaton_t, true>(aut_);
                auto spur_node = filter_aut->src_of(prev[j]);
                auto root_path = path_t(prev.begin(), prev.begin() + j);

                for (const auto& selected_path: res)
                  if (boost::starts_with(selected_path, root_path))
                    filter_aut->hide_transition(selected_path[j]);

                for (auto t: root_path)
                  if (t != filter_aut->null_transition()
                      && filter_aut->src_of(t) != spur_node
                      && filter_aut->src_of(t) != aut_->pre())
                    filter_aut->hide_state(filter_aut->src_of(t));

                auto lightest_path
                  = compute_lightest_path(filter_aut, spur_node, dst);
                auto spur_path
                  = path(filter_aut, lightest_path, spur_node, dst);
                root_path.insert(root_path.end(),
                                 spur_path.begin(), spur_path.end());
                if (!root_path.empty()
                    && filter_aut->src_of(root_path.front()) == src
                    && filter_aut->dst_of(root_path.back()) == dst
                    && none_of(heap,
                               [&root_path](const auto& profile)
                               {
                                 return profile.path_ == root_path;
                               }))
                  {
                    auto m
                      = *path_monomial(filter_aut,
                                       format_lightest(filter_aut, root_path),
                                       src, dst);
                    heap.emplace(std::move(root_path), vs_, get_value_(m));
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
      return (detail::yen_impl<Aut, ValueSet, Mul, GetValue>
              (aut, vs, mul, get_value));
    }
  }


  template <Automaton Aut>
  std::vector<path_t_of<Aut>>
  k_lightest_path(const Aut& aut, state_t_of<Aut> src, state_t_of<Aut> dst,
                  unsigned k)
  {
    auto mul = [&aut](auto lhs, transition_t_of<Aut> t)
               {
                 return aut->weightset()->mul(lhs, aut->weight_of(t));
               };
    auto get_value = [](auto m) { return m.second; };
    auto yen = detail::make_yen(aut, *aut->weightset(), mul, get_value);
    return yen(src, dst, k);
  }

  /// A state-indexed vector of predecessor transitions from the path \a path.
  ///
  /// For each state `s`, `res[s]` is `null_transition` if it is not
  /// part of `path` (or it is its beginning), or `res[s]` is the
  /// transition index that points to the incoming transition to `s`
  /// in `path`.
  template <Automaton Aut>
  predecessors_t_of<Aut>
  format_lightest(const Aut& aut, const std::vector<transition_t_of<Aut>>& path)
  {
    auto res = predecessors_t_of<Aut>(states_size(aut), aut->null_transition());
    for (auto t : path)
      if (t != aut->null_transition())
        res[aut->dst_of(t)] = t;
    return res;
  }

  template <Automaton Aut>
  predecessors_t_of<Aut>
  lightest_path(const Aut& aut, state_t_of<Aut> src, state_t_of<Aut> dst,
                yen_tag)
  {
    auto paths = k_lightest_path(aut, src, dst, 1);
    auto res = paths.empty() ? predecessors_t_of<Aut>() : paths.front();
    return format_lightest(aut, res);
  }
}
