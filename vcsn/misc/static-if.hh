#pragma once

#include <utility>

#include <vcsn/misc/type_traits.hh>

namespace vcsn
{
  namespace detail
  {
    /// Execute the then-clause.
    template <typename Then, typename Else>
    auto static_if_impl(std::true_type, Then&& then, Else&&)
    {
      return std::forward<Then>(then);
    }

    /// Execute the else-clause.
    template <typename Then, typename Else>
    auto static_if_impl(std::false_type, Then&&, Else&& else_)
    {
      return std::forward<Else>(else_);
    }

    /// Execute the then- or the else-clause depending on \a cond.
    template <bool cond, typename Then, typename Else>
    auto static_if(Then&& then, Else&& else_)
    {
      return static_if_impl(bool_constant<cond>{},
                            std::forward<Then>(then),
                            std::forward<Else>(else_));
    }

    /// Execute the then-clause if \a cond is verified.
    template <bool cond, typename Then>
    auto static_if(Then&& then)
    {
      // Applies to both GCC and Clang.  Cannot be done inside a
      // statement.
#if defined __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
      return static_if<cond>(std::forward<Then>(then),
                             [](auto&&...){});
#if defined __GNUC__
# pragma GCC diagnostic pop
#endif
    }
  }
}
