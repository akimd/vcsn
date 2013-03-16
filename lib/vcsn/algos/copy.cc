#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /*------.
  | copy. |
  `------*/

  namespace dyn
  {
    REGISTER_DEFINE(copy);

    automaton
    copy(const automaton& aut)
    {
      return details::copy_registry().call(aut->vname(), aut);
    }
  }
}
