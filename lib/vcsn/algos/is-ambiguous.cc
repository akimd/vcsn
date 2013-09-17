#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/is-ambiguous.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  /*---------------.
  | is_ambiguous.  |
  `---------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_ambiguous);

    bool
    is_ambiguous(const automaton& aut)
    {
      return detail::is_ambiguous_registry().call(aut->vname(false), aut);
    }
  }
}
