#pragma once

#include <vcsn/misc/getargs.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/a-star.hh>

namespace vcsn
{
  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, const std::string& algo)
  {
    enum algorithm
    {
      bellmanford_t,
      dijkstra_t,
      astar_t,
    };
    static const auto map = std::map<std::string, algorithm>
    {
      {"auto",          dijkstra_t},
      {"bellman-ford",  bellmanford_t},
      {"dijkstra",      dijkstra_t},
      {"a-star",        astar_t},
    };
    switch (getargs("algorithm", map, algo))
    {
      case bellmanford_t:
        return bellman_ford(aut);
      case dijkstra_t:
        return dijkstra(aut);
      case astar_t:
        return a_star(aut);
    }
  }
}
