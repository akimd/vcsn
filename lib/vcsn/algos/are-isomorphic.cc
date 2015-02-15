#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(are_isomorphic);
    bool
    are_isomorphic(const automaton& lhs, const automaton& rhs)
    {
      return detail::are_isomorphic_registry().call(lhs, rhs);
    }
  }
}
