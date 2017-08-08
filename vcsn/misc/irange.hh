#pragma once

#include <boost/range/irange.hpp>

namespace vcsn
{
  namespace detail
  {
    /// Generate an integer range.
    ///
    /// Benches in -O3 show no difference with a traditional for loop.
    template <typename Integer>
    auto irange(Integer last)
    {
      return boost::irange(Integer(0), last);
    }
  }
}
