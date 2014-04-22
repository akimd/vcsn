#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/compose.hh>
#include <vcsn/dyn/algos.hh>


namespace vcsn
{

  namespace dyn
  {
    REGISTER_DEFINE(compose);
    automaton
    compose(const automaton& lhs, const automaton& rhs)
    {
      return detail::compose_registry().call(lhs, rhs);
    }

  }
}
