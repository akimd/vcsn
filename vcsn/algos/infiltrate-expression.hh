#pragma once

#include <vcsn/algos/to-expression-expansion.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*----------------------------.
  | infiltrate(value, value).   |
  `----------------------------*/

  /// Infiltration product of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  infiltrate(const ValueSet& vs,
               const typename ValueSet::value_t& lhs,
               const typename ValueSet::value_t& rhs)
  {
    return vs.infiltrate(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      template <typename ValueSetLhs, typename ValueSetRhs,
                typename Value>
      Value
      infiltrate_value(const Value& lhs, const Value& rhs)
      {
        auto joined = join<ValueSetLhs, ValueSetRhs>(lhs, rhs);
        return {joined.valueset,
                ::vcsn::infiltrate(joined.valueset,
                                   joined.lhs,
                                   joined.rhs)};
      }
    }
  }


  /*--------------------------------------.
  | infiltrate(expression, expression).   |
  `--------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (infiltrate).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      infiltrate_expression(const expression& lhs, const expression& rhs)
      {
        return infiltrate_value<ExpSetLhs, ExpSetRhs>(lhs, rhs);
      }
    }
  }

  /*--------------------------------------.
  | infiltrate(polynomial, polynomial).   |
  `--------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (infiltrate).
      template <typename SetLhs, typename SetRhs>
      polynomial
      infiltrate_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        return infiltrate_value<SetLhs, SetRhs>(lhs, rhs);
      }
    }
  }
}
