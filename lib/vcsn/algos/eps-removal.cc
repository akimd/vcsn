#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/eps-removal.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | eps-removal. |
  `-------------*/

  namespace dyn
  {
    REGISTER_DEFINE(eps_removal);

    automaton
    eps_removal(const automaton& aut)
    {
      return detail::eps_removal_registry().call(aut->vname(), aut);
    }
  }
}
