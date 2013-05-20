#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/is_complete.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*--------------.
  | is_complete.  |
  `--------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_complete);

    bool is_complete(const automaton& aut)
    {
      return detail::is_complete_registry().call(aut->vname(),
          aut);
    }
  }
}
