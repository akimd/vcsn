#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(union_a);

    automaton
    union_a(const automaton& lhs, const automaton& rhs)
    {
      return detail::union_a_registry().call(lhs, rhs);
    }
  }
}
