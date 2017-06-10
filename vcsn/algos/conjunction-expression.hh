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
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::conjunction(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
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
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::conjunction(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
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
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::conjunction(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
      }
    }
  }
}
