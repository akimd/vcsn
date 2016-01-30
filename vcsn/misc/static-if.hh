#pragma once

namespace vcsn
{
  namespace detail
  {
    /// Execute the then-clause.
    template <typename Then, typename Else>
    auto static_if(std::true_type, Then then, Else)
    {
      return then;
    }

    /// Execute the else-clause.
    template <typename Then, typename Else>
    auto static_if(std::false_type, Then, Else else_)
    {
      return else_;
    }

    /// Execute the then- or the else-clause depending on cond.
    template <bool cond, typename Then, typename Else>
    auto static_if(Then&& then, Else&& else_)
    {
      return static_if(bool_constant<cond>{},
                       std::forward<Then>(then),
                       std::forward<Else>(else_));
    }
  }
}
