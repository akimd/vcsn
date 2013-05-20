#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/accessible.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | accessible.  |
  `-------------*/

  namespace dyn
  {
    REGISTER_DEFINE(accessible);

    automaton
    accessible(const automaton& aut)
    {
      return detail::accessible_registry().call(aut->vname(),
                                                  aut);
    }
  }
}
