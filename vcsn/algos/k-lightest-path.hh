#pragma once

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/dijkstra.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/algos/filter.hh>

namespace vcsn
{
  struct yen_tag {};

  namespace detail
  {
    template <Automaton Aut>
    struct yen_impl
    {
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using path_t = std::vector<transition_t>;
      using paths_t = std::vector<path_t>;

      using wordset_context_t = word_context_t<context_t>;
      using polynomialset_t = polynomialset<wordset_context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      yen_impl(const automaton_t& aut)
        : aut_(aut)
      {}

      struct profile
      {
        profile(const path_t& path, const monomial_t& m, const polynomialset_t& ps)
          : path_(path)
          , m_(m)
          , ps_(ps)
        {}

        bool operator<(const profile& rhs) const
        {
          if (ps_.weightset()->less(rhs.m_.second, m_.second))
            return true;
          else if (ps_.weightset()->less(m_.second, rhs.m_.second))
            return false;
          else
            return ps_.labelset()->less(rhs.m_.first, m_.first);
        }

        path_t path_;
        monomial_t m_;
        const polynomialset_t& ps_;
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
            res.insert(res.begin(), t);
            if (aut->src_of(t) == src)
              break;
          }
        return res;
      }

      paths_t
      operator()(state_t src, state_t dst, unsigned k)
      {
        auto all_states = aut_->all_states();
        auto ss = std::unordered_set<state_t>(all_states.begin(), all_states.end());
        auto all_trans = aut_->all_transitions();
        auto ts = std::unordered_set<transition_t>(all_trans.begin(), all_trans.end());
        auto res = paths_t{path(aut_, lightest_path(aut_, src, dst, dijkstra_tag{}), src, dst)};
        auto ps = make_word_polynomialset(aut_->context());

        auto heap = heap_t();

        for (unsigned i = 1u; i < k; i++)
          {
            const auto& prev = res[i - 1];
            for (unsigned j = 0u; j < prev.size(); j++)
              {
                auto filter_aut = filter<automaton_t, true>(aut_, ss, ts);
                auto spur_node = filter_aut->src_of(prev[j]);
                auto root_path = path_t(prev.begin(), prev.begin() + j);

                for (const auto& selected_path: res)
                  if (j < selected_path.size())
                    {
                      auto diff = std::mismatch(root_path.begin(), root_path.end(),
                                                selected_path.begin(), selected_path.begin() + j);
                      if (diff.first == root_path.end()
                          && filter_aut->has_transition(selected_path[j]))
                        filter_aut->hide_trans(selected_path[j]);
                    }

                for (auto t: root_path)
                  if (t != filter_aut->null_transition()
                      && filter_aut->src_of(t) != spur_node
                      && filter_aut->src_of(t) != aut_->pre()
                      && filter_aut->has_state(filter_aut->src_of(t)))
                    filter_aut->hide_state(filter_aut->src_of(t));

                auto shortest_path = lightest_path(filter_aut, spur_node, dst, dijkstra_tag{});
                auto spur_path = path(filter_aut, shortest_path, spur_node, dst);
                root_path.insert(root_path.end(), spur_path.begin(), spur_path.end());
                if (!root_path.empty()
                    && filter_aut->src_of(root_path.front()) == src
                    && filter_aut->dst_of(root_path.back()) == dst)
                  {
                    auto m = *path_monomial(filter_aut, format_lightest(filter_aut, root_path), src, dst);
                    heap.emplace(std::move(root_path), m, ps);
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
    };
  }

  template <Automaton Aut>
  std::vector<std::vector<transition_t_of<Aut>>>
  k_lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut> dest, unsigned k)
  {
    return detail::yen_impl<Aut>(aut)(source, dest, k);
  }

  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  format_lightest(const Aut& aut, const std::vector<transition_t_of<Aut>>& path)
  {
    auto res = std::vector<transition_t_of<Aut>>(aut->all_states().back() + 1,
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
