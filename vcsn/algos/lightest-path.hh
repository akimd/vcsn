#pragma once

#include <vcsn/misc/getargs.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/a-star.hh>

namespace vcsn
{
  /*--------------------------------.
  | Shortest path algorithm tags.   |
  `--------------------------------*/

  /// A-Star implementation (from vcsn/algos/a-star.hh).
  ///
  /// Uses neutral heuristic.
  /// No preconditions.
  struct astar_tag {};

  /// Bellman-Ford implementation (from vcsn/algos/bellman-ford.hh).
  ///
  /// Raise if the automaton contains a negative loop.
  struct bellmanford_tag {};

  /// Dijkstra implementation (from vcsn/algos/dijkstra.hh).
  ///
  /// Uses fibonacci heap.
  /// No preconditions.
  struct dijkstra_tag {};

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, astar_tag)
  {
    return a_star(aut);
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, bellmanford_tag)
  {
    return bellman_ford(aut);
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, dijkstra_tag = {})
  {
    return dijkstra(aut);
  }

  template <typename Aut>
  std::vector<transition_t_of<Aut>>
  lightest_path(const Aut& aut, const std::string& algo)
  {
    using path_t = std::vector<transition_t_of<Aut>>;
    static const auto map
      = std::map<std::string, std::function<path_t(const Aut&)>>
    {
      {"auto",
       [](const Aut& a){ return lightest_path(a); }},
      {"a-star",
       [](const Aut& a){ return lightest_path(a, astar_tag{}); }},
      {"bellman-ford",
       [](const Aut& a){ return lightest_path(a, bellmanford_tag{}); }},
      {"dijkstra",
       [](const Aut& a){ return lightest_path(a, dijkstra_tag{}); }},
    };
    auto fun = getargs("algorithm", map, algo);
    return fun(aut);
  }
}
