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
      return detail::copy_registry().call(aut->vname(), aut);
    }
  }
}
