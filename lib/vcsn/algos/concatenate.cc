#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/weight.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(concatenate);
    automaton
    concatenate(const automaton& lhs, const automaton& rhs)
    {
      return detail::concatenate_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(chain);
    automaton
    chain(const automaton& aut, int min, int max)
    {
      return detail::chain_registry().call(aut, min, max);
    }

    REGISTER_DEFINE(concatenate_expression);
    expression
    concatenate(const expression& lhs, const expression& rhs)
    {
      return detail::concatenate_expression_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(chain_expression);
    expression
    chain(const expression& r, int min, int max)
    {
      return detail::chain_expression_registry().call(r, min, max);
    }

    REGISTER_DEFINE(concatenate_polynomial);
    polynomial
    concatenate(const polynomial& lhs, const polynomial& rhs)
    {
      return detail::concatenate_polynomial_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(multiply_weight);
    weight
    multiply(const weight& lhs, const weight& rhs)
    {
      return detail::multiply_weight_registry().call(lhs, rhs);
    }
  }
}
