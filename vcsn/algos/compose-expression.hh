#pragma once

#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*-------------------------.
  | compose(Value, Value).   |
  `-------------------------*/

  /// Composition of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  compose(const ValueSet& vs,
          const typename ValueSet::value_t& lhs,
          const typename ValueSet::value_t& rhs)
  {
    return vs.compose(lhs, rhs);
  }


  /*---------------------------------.
  | compose(expansion, expansion).   |
  `---------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expansion
      compose_expansion(const expansion& lhs, const expansion& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::compose(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }

  /*-----------------------------------.
  | compose(expression, expression).   |
  `-----------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      compose_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::compose(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }

  /*-----------------------------------.
  | compose(polynomial, polynomial).   |
  `-----------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename PolSetLhs, typename PolSetRhs>
      polynomial
      compose_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolSetLhs, PolSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::compose(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }
}
