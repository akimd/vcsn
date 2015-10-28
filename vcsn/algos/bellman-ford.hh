#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/misc/set.hh>

namespace vcsn
{

  /*-----------------------------------------------.
  | Shortest path through Bellman-Ford algorithm.  |
  `-----------------------------------------------*/

  namespace detail
  {
    /// Bellman-Ford implementation of lightest automaton.
    ///
    /// Function applying Bellman-Ford algorithm to the automaton
    /// given as parameter. Return the array of lightest 'in'
    /// transition of each state.
    template <typename Aut>
    boost::optional<std::vector<transition_t_of<Aut>>>
    bellman_ford_impl(const Aut& aut)
    {
      using transition_t = transition_t_of<Aut>;
      auto size = aut->all_states().back() + 1;
      auto dist = std::vector<weight_t_of<Aut>>(size);
      auto res = std::vector<transition_t>(size, aut->null_transition());
      auto ws = *aut->weightset();

      dist[aut->pre()] = ws.one();

      // Iterate one time for each state over each transitions.
      for (auto _: aut->all_states())
        for (auto t: aut->all_transitions())
          {
            auto src = aut->src_of(t);

            if (res[src] != aut->null_transition() || src == aut->pre())
              {
                auto dst = aut->dst_of(t);
                auto nw = ws.mul(dist[src], aut->weight_of(t));
                if (res[dst] == aut->null_transition()
                    || ws.less(nw, dist[dst]))
                  {
                    dist[dst] = nw;
                    res[dst] = t;
                  }
              }
          }

      // Check for negative cycles.
      for (auto t: aut->transitions())
        {
          auto src = aut->src_of(t);
          auto dst = aut->dst_of(t);
          if (res[src] != aut->null_transition()
              && (res[dst] == aut->null_transition()
                 || ws.less(ws.mul(dist[src], aut->weight_of(t)), dist[dst])))
            return boost::none;
        }

      return std::move(res);
    }
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  bellman_ford(const Aut& aut)
  {
    auto bf = bellman_ford_impl(aut);
    require(bf != boost::none, "bellman-ford: automaton with negative cycle");
    return std::move(*bf);
  }
}
