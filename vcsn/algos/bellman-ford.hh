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
      auto l = std::vector<boost::optional<weight_t>>(size);
      auto res = std::vector<state_t>(size);
      auto ws = *aut->weightset();

      l[aut->pre()] = ws.one();

      // Iterate one time for each state over each transitions.
      for (auto _: aut->all_states())
        for (auto t: aut->all_transitions())
          {
            auto src = aut->src_of(t);

            if (l[src])
              {
                auto dst = aut->dst_of(t);
                auto nw = ws.mul(*l[src], aut->weight_of(t));
                if (!l[dst] || ws.less(nw, *l[dst]))
                  {
                    l[dst] = nw;
                    res[dst] = src;
                  }
              }
          }

      // Check for negative cycles.
      for (auto t: aut->transitions())
        {
          auto src = aut->src_of(t);
          auto dst = aut->dst_of(t);
          if (l[src]
              && (!l[dst]
                 || ws.less(ws.mul(*l[src], aut->weight_of(t)), *l[dst])))
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
