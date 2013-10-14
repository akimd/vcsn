#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/union.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(union_a);

    automaton
    union_a(const automaton& lhs, const automaton& rhs)
    {
      return detail::union_a_registry().call(vname(lhs, rhs), lhs, rhs);
    }
  }
}
