#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/dyn/algos.hh>

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
      return detail::is_deterministic_registry().call(aut);
    }


    REGISTER_DEFINE(is_codeterministic);

    bool
    is_codeterministic(const automaton& aut)
    {
      return detail::is_codeterministic_registry().call(aut);
    }
  }
}
