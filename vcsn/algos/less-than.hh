#pragma once

#include <stdexcept>

#include <vcsn/algos/constant-term.hh>

namespace vcsn
{

  /*-------------------------------------.
  | less_than(expression, expression).   |
  `-------------------------------------*/

  /// Whether lhs precedes rhs.
  template <typename ExpSet>
  bool
  less_than(const ExpSet& rs,
            const typename ExpSet::value_t& lhs,
            const typename ExpSet::value_t& rhs)
  {
    return rs.less(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (less_than).
      template <typename ExpSet1, typename ExpSet2>
      bool
      less_than_expression(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSet1>();
        const auto& r = rhs->as<ExpSet2>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return ::vcsn::less_than(rs, lr, rr);
      }
    }
  }
} // namespace vcsn
