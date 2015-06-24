#pragma once

#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/polynomial.hh>

namespace vcsn
{

  /*----------------------.
  | ldiv(value, value).   |
  `----------------------*/

  /// Left-division of values.
  template <typename ValueSet>
  inline
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
        const auto& l = lhs->as<ExpressionSetLhs>();
        const auto& r = rhs->as<ExpressionSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return make_expression(rs, ldiv(rs, lr, rr));
      }

      /// Bridge (ldiv).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      ldiv_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        const auto& l = lhs->as<PolynomialSetLhs>();
        const auto& r = rhs->as<PolynomialSetRhs>();
        auto rs = join(l.polynomialset(), r.polynomialset());
        auto lr = rs.conv(l.polynomialset(), l.polynomial());
        auto rr = rs.conv(r.polynomialset(), r.polynomial());
        return make_polynomial(rs, ldiv(rs, lr, rr));
      }
    }
  }

  /*--------------------------------.
  | lgcd(polynomial, polynomial).   |
  `--------------------------------*/

  /// Left-division of values.
  template <typename ValueSet>
  inline
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
        const auto& l = lhs->as<PolynomialSetLhs>();
        const auto& r = rhs->as<PolynomialSetRhs>();
        auto rs = join(l.polynomialset(), r.polynomialset());
        auto lr = rs.conv(l.polynomialset(), l.polynomial());
        auto rr = rs.conv(r.polynomialset(), r.polynomial());
        return make_polynomial(rs, lgcd(rs, lr, rr));
      }
    }
  }

  /*----------------------.
  | rdiv(value, value).   |
  `----------------------*/

  /// Right-division of values.
  template <typename ValueSet>
  inline
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
        const auto& l = lhs->as<ExpressionSetLhs>();
        const auto& r = rhs->as<ExpressionSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return make_expression(rs, rdiv(rs, lr, rr));
      }
    }
  }
}
