#pragma once

#include <optional>

#include <vcsn/core/automaton.hh> // states_size
#include <vcsn/misc/set.hh>
#include <vcsn/weightset/fwd.hh>

namespace vcsn
{
  /*-----------------------------------------------.
  | Shortest path through Bellman-Ford algorithm.  |
  `-----------------------------------------------*/

  /// Bellman-Ford implementation (from vcsn/algos/bellman-ford.hh).
  ///
  /// Raise if the automaton contains a lightening loop.
  struct bellman_ford_tag {};

  namespace detail
  {
    /// Bellman-Ford implementation of lightest automaton.
    ///
    /// Function applying Bellman-Ford algorithm to the automaton
    /// given as parameter. Return the array of lightest 'in'
    /// transition of each state.
    template <Automaton Aut>
    std::optional<predecessors_t_of<Aut>>
    bellman_ford_impl(const Aut& aut, state_t_of<Aut> source)
    {
      auto size = states_size(aut);
      auto dist = std::vector<weight_t_of<Aut>>(size);
      auto res = predecessors_t_of<Aut>(size, aut->null_transition());
      auto ws = *aut->weightset();

      dist[source] = ws.one();

      // Iterate once for each state over each transitions.
      for (auto _: aut->all_states())
        {
          (void) _;
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
        }

      // Check for lightening cycles.
      auto has_lightening_cycle = any_of(transitions(aut), [&](auto t) {
          auto src = aut->src_of(t);
          auto dst = aut->dst_of(t);
          return (res[src] != aut->null_transition()
                  && (res[dst] == aut->null_transition()
                      || ws.less(ws.mul(dist[src], aut->weight_of(t)),
                                 dist[dst])));
        });
      if (has_lightening_cycle)
        return std::nullopt;
      else
        return res;
    }
  }

  /// Destination is ignored as bellman-ford does not stop when reaching dest,
  /// but when each iteration has been done.
  template <Automaton Aut>
  predecessors_t_of<Aut>
  lightest_path(const Aut& aut, state_t_of<Aut> source, state_t_of<Aut>,
                bellman_ford_tag)
  {
    auto res = bellman_ford_impl(aut, source);
    require(res, "bellman-ford: automaton with negative cycle");
    return std::move(*res);
  }
}
