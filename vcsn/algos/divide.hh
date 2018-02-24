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
        auto joined = join<ExpansionSetLhs, ExpansionSetRhs>(lhs, rhs);
        return {joined.valueset,
                ldivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }

      /// Bridge (ldivide).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      ldivide_expression(const expression& lhs, const expression& rhs)
      {
        auto joined = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {joined.valueset,
                ldivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }

      /// Bridge (ldivide).
      template <typename LabelSetLhs, typename LabelSetRhs>
      label
      ldivide_label(const label& lhs, const label& rhs)
      {
        auto joined = join<LabelSetLhs, LabelSetRhs>(lhs, rhs);
        return {joined.valueset,
                ldivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }

      /// Bridge (ldivide).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      ldivide_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto joined = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {joined.valueset,
                ldivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }

      /// Bridge (ldivide).
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      ldivide_weight(const weight& lhs, const weight& rhs)
      {
        auto joined = join<WeightSetLhs, WeightSetRhs>(lhs, rhs);
        return {joined.valueset,
                ldivide(joined.valueset,
                        joined.lhs, joined.rhs)};
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
        auto joined = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {joined.valueset,
                lgcd(joined.valueset,
                     joined.lhs, joined.rhs)};
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
        auto joined = join<LabelSetLhs, LabelSetRhs>(lhs, rhs);
        return {joined.valueset,
                rdivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }

      /// Bridge (rdivide).
      template <typename ExpressionSetLhs, typename ExpressionSetRhs>
      expression
      rdivide_expression(const expression& lhs, const expression& rhs)
      {
        auto joined = join<ExpressionSetLhs, ExpressionSetRhs>(lhs, rhs);
        return {joined.valueset,
                rdivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }

      /// Bridge (rdivide).
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      rdivide_weight(const weight& lhs, const weight& rhs)
      {
        auto joined = join<WeightSetLhs, WeightSetRhs>(lhs, rhs);
        return {joined.valueset,
                rdivide(joined.valueset,
                        joined.lhs, joined.rhs)};
      }
    }
  }
}
