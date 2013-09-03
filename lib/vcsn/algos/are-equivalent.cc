#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/are-equivalent.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(are_equivalent);

    bool
    are_equivalent(const automaton& lhs, const automaton& rhs)
    {
      return
        detail::are_equivalent_registry().call
        (lhs->vname(false) + " x " + rhs->vname(false),
         lhs, rhs);
    }

    bool
    are_equivalent(const ratexp& lhs, const ratexp& rhs)
    {
      return are_equivalent(standard(lhs), standard(rhs));
    }
  }
}
