#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/misc/set.hh>

namespace vcsn
{
  /*-----------------------------------------------.
  | Shortest path through Bellman-Ford algorithm.  |
  `-----------------------------------------------*/

  /// Bellman-Ford implementation (from vcsn/algos/bellman-ford.hh).
  ///
  /// Raise if the automaton contains a lightening loop.
  struct bellmanford_tag {};

  namespace detail
  {
    /// Bellman-Ford implementation of lightest automaton.
    ///
    /// Function applying Bellman-Ford algorithm to the automaton
    /// given as parameter. Return the array of lightest 'in'
    /// transition of each state.
    template <Automaton Aut>
    boost::optional<std::vector<transition_t_of<Aut>>>
    bellman_ford_impl(const Aut& aut, state_t_of<Aut> source)
    {
      using transition_t = transition_t_of<Aut>;
      auto size = aut->all_states().back() + 1;
      auto dist = std::vector<weight_t_of<Aut>>(size);
      auto res = std::vector<transition_t>(size, aut->null_transition());
      auto ws = *aut->weightset();

      dist[source] = ws.one();

      // Iterate one time for each state over each transitions.
      for (auto _: aut->all_states())
        for (auto t: all_transitions(aut))
          {
            auto src = aut->src_of(t);

            if (res[src] != aut->null_transition() || src == source)
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

      // Check for lightening cycles.
      for (auto t: transitions(aut))
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

  /// Destination is ignored as bellman-ford does not stop when reaching dest,
  /// but when each iteration has been done.
  template <Automaton Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut>,
                bellmanford_tag)
  {
    auto res = bellman_ford_impl(aut, source);
    require(res, "bellman-ford: automaton with negative cycle");
    return std::move(*res);
  }
}
