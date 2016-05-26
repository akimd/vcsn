#pragma once

#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*----------------------.
  | ldiv(value, value).   |
  `----------------------*/

  /// Left-division of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  ldiv(const ValueSet& vs,
       const typename ValueSet::value_t& lhs,
       const typename ValueSet::value_t& rhs)
  {
    return vs.ldiv(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (ldiv).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      ldiv_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), ldiv(std::get<0>(join_elts),
                                             std::get<1>(join_elts),
                                             std::get<2>(join_elts))};
      }

      /// Bridge (ldiv).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      ldiv_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), ldiv(std::get<0>(join_elts),
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

  /*----------------------.
  | rdiv(value, value).   |
  `----------------------*/

  /// Right-division of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  rdiv(const ValueSet& vs,
       const typename ValueSet::value_t& lhs,
       const typename ValueSet::value_t& rhs)
  {
    return vs.rdiv(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (rdiv).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      rdiv_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts), rdiv(std::get<0>(join_elts),
                                             std::get<1>(join_elts),
                                             std::get<2>(join_elts))};
      }
    }
  }
}
