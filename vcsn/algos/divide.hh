#pragma once

#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*-------------------------.
  | ldivide(value, value).   |
  `-------------------------*/

  /// Left-division of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  ldivide(const ValueSet& vs,
       const typename ValueSet::value_t& lhs,
       const typename ValueSet::value_t& rhs)
  {
    return vs.ldivide(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (ldivide).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      ldivide_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), ldivide(std::get<0>(join_elts),
                                             std::get<1>(join_elts),
                                             std::get<2>(join_elts))};
      }

      /// Bridge (ldivide).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      ldivide_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), ldivide(std::get<0>(join_elts),
                                             std::get<1>(join_elts),
                                             std::get<2>(join_elts))};
      }
    }
  }

  /*--------------------------------.
  | lgcd(polynomial, polynomial).   |
  `--------------------------------*/

  /// Left-division of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  lgcd(const ValueSet& vs,
       const typename ValueSet::value_t& lhs,
       const typename ValueSet::value_t& rhs)
  {
    return vs.lgcd(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (lgcd).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      lgcd_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), lgcd(std::get<0>(join_elts),
                                             std::get<1>(join_elts),
                                             std::get<2>(join_elts))};
      }
    }
  }

  /*-------------------------.
  | rdivide(value, value).   |
  `-------------------------*/

  /// Right-division of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  rdivide(const ValueSet& vs,
       const typename ValueSet::value_t& lhs,
       const typename ValueSet::value_t& rhs)
  {
    return vs.rdivide(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (rdivide).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      rdivide_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), rdivide(std::get<0>(join_elts),
                                             std::get<1>(join_elts),
                                             std::get<2>(join_elts))};
      }
    }
  }
}
