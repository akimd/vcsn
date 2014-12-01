#pragma once

#include <vcsn/dyn/polynomial.hh>

namespace vcsn
{

  /*--------------------------------.
  | ldiv(polynomial, polynomial).   |
  `--------------------------------*/

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
      /// Bridge.
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

      REGISTER_DECLARE(ldiv_polynomial,
                       (const polynomial&, const polynomial&) -> polynomial);
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
      /// Bridge.
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

      REGISTER_DECLARE(lgcd_polynomial,
                       (const polynomial&, const polynomial&) -> polynomial);
    }
  }
}
