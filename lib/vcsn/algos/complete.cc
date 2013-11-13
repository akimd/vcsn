#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/complete.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

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
      return detail::complete_registry().call(aut);
    }
  }
}
