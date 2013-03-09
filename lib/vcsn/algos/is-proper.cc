#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/eps-removal.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------------.
    | is_proper.  |
    `------------*/

    REGISTER_DEFINE(is_proper);

    bool
    is_proper(const automaton& aut)
    {
      return details::is_proper_registry().call(aut->vname(), aut);
    }
  }
}
