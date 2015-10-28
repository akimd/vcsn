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
    template <typename Aut>
    boost::optional<std::vector<state_t_of<Aut>>>
    bellman_ford_impl(const Aut& aut)
    {
      using weight_t = weight_t_of<Aut>;
      using state_t = state_t_of<Aut>;
      auto size = aut->all_states().back() + 1;
      auto dist = std::vector<weight_t>(size);
      auto res = std::vector<state_t>(size, aut->null_state());
      auto ws = *aut->weightset();

      dist[aut->pre()] = ws.one();

      // Iterate one time for each state over each transitions.
      for (auto _: aut->all_states())
        for (auto t: aut->all_transitions())
          {
            auto src = aut->src_of(t);

            if (res[src] != aut->null_state() || src == aut->pre())
              {
                auto dst = aut->dst_of(t);
                auto nw = ws.mul(dist[src], aut->weight_of(t));
                if (res[dst] == aut->null_state() || ws.less(nw, dist[dst]))
                  {
                    dist[dst] = nw;
                    res[dst] = src;
                  }
              }
          }

      // Check for negative cycles.
      for (auto t: aut->transitions())
        {
          auto src = aut->src_of(t);
          auto dst = aut->dst_of(t);
          if (res[src] != aut->null_state()
              && (res[dst] == aut->null_state()
                 || ws.less(ws.mul(dist[src], aut->weight_of(t)), dist[dst])))
            return boost::none;
        }

      return std::move(res);
    }
  }

  template <typename Aut>
  std::vector<state_t_of<Aut>>
  bellman_ford(const Aut& aut)
  {
    auto bf = bellman_ford_impl(aut);
    require(bf != boost::none, "bellman-ford: automaton with negative cycle");
    return std::move(*bf);
  }
}
