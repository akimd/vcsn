#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*-----------------------------.
  | conjunction(value, value).   |
  `-----------------------------*/

  /// Intersection/Hadamard product of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  conjunction(const ValueSet& rs,
              const typename ValueSet::value_t& lhs,
              const typename ValueSet::value_t& rhs)
  {
    return rs.conjunction(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      template <typename ValueSetLhs, typename ValueSetRhs,
                typename Value>
      Value
      conjunction_value(const Value& lhs, const Value& rhs)
      {
        auto joined = join<ValueSetLhs, ValueSetRhs>(lhs, rhs);
        return {joined.valueset,
                ::vcsn::conjunction(joined.valueset,
                                    joined.lhs,
                                    joined.rhs)};
      }
    }
  }

  /*-------------------------------------.
  | conjunction(expansion, expansion).   |
  `-------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expansion
      conjunction_expansion(const expansion& lhs, const expansion& rhs)
      {
        return conjunction_value<ExpSetLhs, ExpSetRhs>(lhs, rhs);
      }
    }
  }

  /*---------------------------------------.
  | conjunction(expression, expression).   |
  `---------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      conjunction_expression(const expression& lhs, const expression& rhs)
      {
        return conjunction_value<ExpSetLhs, ExpSetRhs>(lhs, rhs);
      }
    }
  }

  /*---------------------------------------.
  | conjunction(polynomial, polynomial).   |
  `---------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      conjunction_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        return conjunction_value<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
      }
    }
  }
}
