#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/dyn/algos.hh>

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
      return detail::is_proper_registry().call(aut->vname(), aut);
    }
  }
}
