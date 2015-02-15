#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/weight.hh>

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

    REGISTER_DEFINE(sum_polynomial);
    polynomial
    sum(const polynomial& lhs, const polynomial& rhs)
    {
      return detail::sum_polynomial_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(sum_expression);
    expression
    sum(const expression& lhs, const expression& rhs)
    {
      return detail::sum_expression_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(sum_weight);
    weight
    sum(const weight& lhs, const weight& rhs)
    {
      return detail::sum_weight_registry().call(lhs, rhs);
    }
  }
}
