#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  /*-------------------.
  | is_deterministic.  |
  `-------------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_deterministic);

    bool
    is_deterministic(const automaton& aut)
    {
      return details::is_deterministic_registry().call(aut->vname(), aut);
    }
  }
}
