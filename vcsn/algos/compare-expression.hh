#pragma once

namespace vcsn
{

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (are_equal).
      template <typename ExpSet1, typename ExpSet2>
      bool
      are_equal_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSet1, ExpSet2>(lhs, rhs);
        return std::get<0>(join_elts).equal(std::get<1>(join_elts),
                                            std::get<2>(join_elts));
      }

      /// Bridge (less_than).
      template <typename ExpSet1, typename ExpSet2>
      bool
      less_than_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSet1, ExpSet2>(lhs, rhs);
        return std::get<0>(join_elts).less(std::get<1>(join_elts),
                                           std::get<2>(join_elts));
      }
    }
  }
} // namespace vcsn
