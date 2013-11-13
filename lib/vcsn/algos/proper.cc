#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*--------.
  | proper. |
  `--------*/

  namespace dyn
  {
    REGISTER_DEFINE(proper);

    automaton
    proper(const automaton& aut)
    {
      return detail::proper_registry().call(aut);
    }
  }
}
