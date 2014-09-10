#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/is-functional.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*----------------.
  | is-functional.  |
  `----------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_functional);

    bool is_functional(const automaton& aut)
    {
      return detail::is_functional_registry().call(aut);
    }
  }
}
