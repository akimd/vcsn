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
        auto join_elts = join<ValueSetLhs, ValueSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::shuffle(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }


  /*---------------------------------.
  | shuffle(expansion, expansion).   |
  `---------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (shuffle).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expansion
      shuffle_expansion(const expansion& lhs, const expansion& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        const auto& xs = std::get<0>(join_elts);
        const auto& x1 = std::get<1>(join_elts);
        const auto& x2 = std::get<2>(join_elts);
        return {xs,
            xs.shuffle(x1, to_expression(xs, x1),
                       x2, to_expression(xs, x2))};
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
