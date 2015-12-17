#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/lightest-path.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/set.hh>

namespace vcsn
{
  /// Lightest Automaton
  ///
  /// An automaton composed only of the states included in the
  /// lightest path from pre to post. Using the shortest path algorithm
  /// given as parameter to compute the lightest 'in' transition of each
  /// state. Then, reconstruct the path keeping the states and transitions
  /// we met. Now that we have the sets of elements to keep we can construct
  /// a new automaton using these states/transitions.
  template <typename Aut, typename Algo = dijkstra_tag>
  fresh_automaton_t_of<Aut>
  lightest_automaton(const Aut& aut, Algo algo = {})
  {
    require(is_tropical<weightset_t_of<Aut>>(),
            "lightest-automaton: require tropical weightset");
    auto pred = lightest_path(aut, algo);
    auto res = make_fresh_automaton(aut);
    auto copy = make_copier(aut, res);
    for (auto t = pred[aut->post()];
         t != aut->null_transition();
         t = pred[aut->src_of(t)])
      copy(t);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      automaton
      lightest_automaton(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::lightest_automaton(a, algo));
      }
    }
  }
}
