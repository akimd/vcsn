#pragma once

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
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::shuffle(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }
}
