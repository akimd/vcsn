#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/are-equivalent.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(difference);

    automaton
    difference(const automaton& lhs, const automaton& rhs)
    {
      return
        detail::difference_registry().call(vname(lhs, rhs),
                                           lhs, rhs);
    }


    REGISTER_DEFINE2(are_equivalent);

    bool
    are_equivalent(const automaton& lhs, const automaton& rhs)
    {
      return
        detail::are_equivalent_registry().call(vname(lhs, rhs),
                                               lhs, rhs);
    }

    bool
    are_equivalent(const ratexp& lhs, const ratexp& rhs)
    {
      return are_equivalent(standard(lhs), standard(rhs));
    }
  }
}
