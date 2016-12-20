#pragma once

namespace vcsn
{

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compare).
      template <typename ExpSet1, typename ExpSet2>
      int
      compare_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSet1, ExpSet2>(lhs, rhs);
        return std::get<0>(join_elts).compare(std::get<1>(join_elts),
                                              std::get<2>(join_elts));
      }
    }
  }
} // namespace vcsn
