#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/concatenate.hh>
#include <vcsn/dyn/algos.hh>

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

    REGISTER_DEFINE(concatenate_ratexp);
    ratexp
    concatenate(const ratexp& lhs, const ratexp& rhs)
    {
      return detail::concatenate_ratexp_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(chain_ratexp);
    ratexp
    chain(const ratexp& r, int min, int max)
    {
      return detail::chain_ratexp_registry().call(r, min, max);
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
