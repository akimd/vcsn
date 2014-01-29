#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/are-equivalent.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(are_equivalent);
    bool
    are_equivalent(const automaton& lhs, const automaton& rhs)
    {
      return detail::are_equivalent_registry().call(lhs, rhs);
    }

    bool
    are_equivalent(const ratexp& lhs, const ratexp& rhs)
    {
      return are_equivalent(standard(lhs), standard(rhs));
    }

    REGISTER_DEFINE(difference);
    automaton
    difference(const automaton& lhs, const automaton& rhs)
    {
      return detail::difference_registry().call(lhs, rhs);
    }
  }
}
