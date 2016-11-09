#pragma once

#include <boost/heap/fibonacci_heap.hpp>

namespace vcsn
{
  namespace detail
  {
    /// Comparison functor.
    ///
    /// Similar to std::greater but uses operator< instead of operator< which
    /// forced us to define it in Elt classes.
    template <typename Elt>
    struct greater
    {
      bool operator()(const Elt& lhs, const Elt& rhs) const
      {
        return rhs < lhs;
      }
    };

    template <typename Elt>
    using comparator_t = boost::heap::compare<detail::greater<Elt>>;
  }

  template <typename Elt>
  using min_fibonacci_heap
    = boost::heap::fibonacci_heap<Elt, detail::comparator_t<Elt>>;

  template <typename Elt>
  using max_fibonacci_heap = boost::heap::fibonacci_heap<Elt>;
}
