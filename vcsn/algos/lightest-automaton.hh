#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/tags.hh>
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
  template <Automaton Aut, typename Algo = auto_tag>
  fresh_automaton_t_of<Aut>
  lightest_automaton(const Aut& aut, unsigned k, Algo algo = {})
  {
    require(is_tropical<weightset_t_of<Aut>>(),
            "lightest-automaton: ", *aut->weightset(),
            " is not a tropical weightset");
    auto res = make_fresh_automaton(aut);
    // The copy is not 'safe' (uses add_transition instead of new_transition),
    // as there can be the same transition multiple times.
    auto copy = make_copier(aut, res, false);
    if (k == 1)
      {
        auto pred = lightest_path(aut, algo);
        for (auto t = pred[aut->post()];
             t != aut->null_transition();
             t = pred[aut->src_of(t)])
          copy(t);
      }
    else if (1 < k)
      {
        for (auto path : k_lightest_path(aut, aut->pre(), aut->post(), k))
          copy(path);
      }
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Num, typename String>
      automaton
      lightest_automaton(const automaton& aut, unsigned k,
                         const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::lightest_automaton(a, k, algo);
      }
    }
  }
}
