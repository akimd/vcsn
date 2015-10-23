#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/algos/dijkstra.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/getargs.hh>

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
    };
    static const auto map = std::map<std::string, algorithm>
    {
      {"auto",          bellmanford_t},
      {"bellman-ford",  bellmanford_t},
      {"dijkstra",      dijkstra_t},
    };
    switch (getargs("algorithm", map, algo))
    {
      case bellmanford_t:
        return bellman_ford(aut);
      case dijkstra_t:
        return dijkstra(aut);
    }
  }

  template <typename Aut>
  Aut
  lightest_automaton(const Aut& aut, const std::string& algo)
  {
    require(is_tropical<weightset_t_of<Aut>>(),
            "lightest-automaton: require tropical weightset");
    using state_t = state_t_of<Aut>;
    auto pred = lightest_algo(aut, algo);
    auto res = std::set<state_t>{aut->post()};
    for (state_t s = pred[aut->post()]; s != aut->pre(); s = pred[s])
      {
        if (s == aut->null_state())
          {
            res.clear();
            break;
          }
        res.insert(s);
      }
    return copy(aut, res);
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
