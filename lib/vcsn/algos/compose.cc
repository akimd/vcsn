#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(compose);
    automaton
    compose(automaton& lhs, automaton& rhs)
    {
      return detail::compose_registry().call(lhs, rhs);
    }

  }
}
