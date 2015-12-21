#pragma once

#include <tuple>

#include <vcsn/misc/type_traits.hh> // detect

namespace vcsn
{

  namespace detail
  {
    template <typename T>
    using test_t = decltype(std::declval<T>().size());

    /// Whether T features a size() function.
    template <typename T>
    using has_size_mem_fn = detect<T, test_t>;
  }


  /// Military strict order predicate.
  ///
  /// This predicate applies to any type which provides, or does not
  /// provide, a size method.  If two elements have the same size they
  /// are compared with operator<, otherwise, the shortest is the
  /// smallest.
  template <typename T>
  struct MilitaryOrder
  {
  public:
    bool operator()(const T& x, const T& y) const
    {
      return lt_<T>(x, y);
    }

  private:
    /// Case where T features a size() member function.
    template <typename T2>
    auto lt_(const T2& x, const T2& y) const
      -> std::enable_if_t<detail::has_size_mem_fn<T2>::value, bool>
    {
      return
        std::forward_as_tuple(x.size(), x) < std::forward_as_tuple(y.size(), y);
    }

    /// Case where T does not feature a size() member function.
    template <typename T2>
    auto lt_(const T2& x, const T2& y) const
      -> std::enable_if_t<!detail::has_size_mem_fn<T2>::value, bool>
    {
      return x < y;
    }
  };
}
