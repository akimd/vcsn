#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/ratexp.hh>
#include <vcsn/algos/sum.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(sum);

    automaton
    sum(const automaton& lhs, const automaton& rhs)
    {
      return detail::sum_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(sum_ratexp);
    ratexp
    sum(const ratexp& lhs, const ratexp& rhs)
    {
      return detail::sum_ratexp_registry().call(lhs, rhs);
    }
  }
}
