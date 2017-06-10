#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*----------------------------.
  | infiltrate(value, value).   |
  `----------------------------*/

  /// Infiltration product of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  infiltrate(const ValueSet& vs,
               const typename ValueSet::value_t& lhs,
               const typename ValueSet::value_t& rhs)
  {
    return vs.infiltrate(lhs, rhs);
  }

  /*--------------------------------------.
  | infiltrate(expression, expression).   |
  `--------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (infiltrate).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      infiltrate_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::infiltrate(std::get<0>(join_elts),
                                   std::get<1>(join_elts),
                                   std::get<2>(join_elts))};
      }
    }
  }
}
