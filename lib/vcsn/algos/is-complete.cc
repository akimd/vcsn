#include <lib/vcsn/algos/registry.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{

  /*--------------.
  | is-complete.  |
  `--------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_complete);

    bool is_complete(const automaton& aut)
    {
      return detail::is_complete_registry().call(aut);
    }
  }
}
