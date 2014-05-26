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
    REGISTER_DEFINE(is_sequential);
    REGISTER_DEFINE(is_deterministic);

    bool
    is_sequential(const automaton& aut)
    {
      return detail::is_sequential_registry().call(aut);
    }
    bool
    is_deterministic(const automaton& aut)
    {
      return detail::is_deterministic_registry().call(aut);
    }
  }
}
