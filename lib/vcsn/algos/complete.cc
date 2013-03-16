#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/complete.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
    /*--------.
  | complete.  |
  `-----------*/

  namespace dyn
  {
    REGISTER_DEFINE(complete);

    automaton
    complete(const automaton& aut)
    {
      return details::complete_registry().call(aut->vname(), aut);
    }
  }
}
