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
      template <typename ExpansionSetLhs, typename ExpansionSetRhs>
      expansion
      ldivide_expansion(const expansion& lhs, const expansion& rhs)
      {
        auto [vs, l, r] = join<ExpansionSetLhs, ExpansionSetRhs>(lhs, rhs);
        return {vs, ldivide(vs, l, r)};
      }

      /// Bridge (ldivide).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      ldivide_expression(const expression& lhs, const expression& rhs)
      {
        auto [vs, l, r] = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {vs, ldivide(vs, l, r)};
      }

      /// Bridge (ldivide).
      template <typename LabelSetLhs, typename LabelSetRhs>
      label
      ldivide_label(const label& lhs, const label& rhs)
      {
        auto [vs, l, r] = join<LabelSetLhs, LabelSetRhs>(lhs, rhs);
        return {vs, ldivide(vs, l, r)};
      }

      /// Bridge (ldivide).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      ldivide_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto [vs, l, r] = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {vs, ldivide(vs, l, r)};
      }

      /// Bridge (ldivide).
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      ldivide_weight(const weight& lhs, const weight& rhs)
      {
        auto [vs, l, r] = join<WeightSetLhs, WeightSetRhs>(lhs, rhs);
        return {vs, ldivide(vs, l, r)};
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
        auto [vs, l, r] = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {vs, lgcd(vs, l, r)};
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
      template <typename LabelSetLhs, typename LabelSetRhs>
      label
      rdivide_label(const label& lhs, const label& rhs)
      {
        auto [vs, l, r] = join<LabelSetLhs, LabelSetRhs>(lhs, rhs);
        return {vs, rdivide(vs, l, r)};
      }

      /// Bridge (rdivide).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      rdivide_expression(const expression& lhs, const expression& rhs)
      {
        auto [vs, l, r] = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {vs, rdivide(vs, l, r)};
      }

      /// Bridge (rdivide).
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      rdivide_weight(const weight& lhs, const weight& rhs)
      {
        auto [vs, l, r] = join<WeightSetLhs, WeightSetRhs>(lhs, rhs);
        return {vs, rdivide(vs, l, r)};
      }
    }
  }
}
