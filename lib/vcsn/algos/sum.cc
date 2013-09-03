#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/sum.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(sum);

    automaton
    sum(const automaton& lhs, const automaton& rhs)
    {
      return detail::sum_registry().call(rhs->vname(false) + " x " +
                                                 rhs->vname(false), lhs, rhs);
    }
  }
}
