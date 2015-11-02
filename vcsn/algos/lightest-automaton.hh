#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/algos/a-star.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/set.hh>

namespace vcsn
{
  template <typename Aut>
  std::vector<state_t_of<Aut>>
  lightest_algo(const Aut& aut, const std::string& algo)
  {
    enum algorithm
    {
      bellmanford_t,
      dijkstra_t,
      astar_t,
    };
    static const auto map = std::map<std::string, algorithm>
    {
      {"auto",          bellmanford_t},
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

  /// Lightest Automaton
  ///
  /// Return an automaton composed only of the states included in the
  /// lightest path from pre to post. Using the shortest path algorithm
  /// given as parameter to compute the lightest 'in' transition of each
  /// state. Then, reconstruct the path keeping the states and transitions
  /// we met. Now that we have the sets of elements to keep we can construct
  /// a new automaton using these states/transitions.
  template <typename Aut>
  Aut
  lightest_automaton(const Aut& aut, const std::string& algo)
  {
    require(is_tropical<weightset_t_of<Aut>>(),
            "lightest-automaton: require tropical weightset");
    using state_t = state_t_of<Aut>;
    using transition_t = transition_t_of<Aut>;
    auto pred = lightest_algo(aut, algo);
    if (pred[aut->post()] == aut->null_transition())
      return make_fresh_automaton(aut);
    else
      {
        auto keep_tr = std::set<transition_t>{aut->pre()};
        for (transition_t t = pred[aut->post()];
             aut->src_of(t) != aut->pre();
             t = pred[aut->src_of(t)])
          keep_tr.insert(t);
        return copy_path(aut, keep_tr);
      }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge
      template <typename Aut, typename String>
      automaton lightest_automaton(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::lightest_automaton(a, algo));
      }
    }
  }
}