#pragma once

#include <vcsn/algos/to-expression-expansion.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*-------------------------.
  | shuffle(value, value).   |
  `-------------------------*/

  /// Shuffle product of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  shuffle(const ValueSet& vs,
          const typename ValueSet::value_t& lhs,
          const typename ValueSet::value_t& rhs)
  {
    return vs.shuffle(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename ValueSetLhs, typename ValueSetRhs,
                typename Value>
      Value
      shuffle_value(const Value& lhs, const Value& rhs)
      {
        auto [vs, l, r] = join<ValueSetLhs, ValueSetRhs>(lhs, rhs);
        return {vs, ::vcsn::shuffle(vs, l, r)};
      }
    }
  }



  /*-----------------------------------.
  | shuffle(expression, expression).   |
  `-----------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (shuffle).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      shuffle_expression(const expression& lhs, const expression& rhs)
      {
        return shuffle_value<ExpSetLhs, ExpSetRhs>(lhs, rhs);
      }
    }
  }

  /*-----------------------------------.
  | shuffle(polynomial, polynomial).   |
  `-----------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (shuffle).
      template <typename SetLhs, typename SetRhs>
      polynomial
      shuffle_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        return shuffle_value<SetLhs, SetRhs>(lhs, rhs);
      }
    }
  }
}
