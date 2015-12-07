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
        auto join_elts = join<ExpSet1, ExpSet2>(lhs, rhs);
        return ::vcsn::less_than(std::get<0>(join_elts),
                                 std::get<1>(join_elts),
                                 std::get<2>(join_elts));
      }
    }
  }
} // namespace vcsn
