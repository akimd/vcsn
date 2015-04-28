#pragma once

#include <initializer_list>

#include <boost/range/algorithm/find.hpp>

namespace vcsn
{

  /// Whether \a e is member of \a m.
  template <typename T, typename U>
  inline
  bool
  has(const std::initializer_list<T>& m, const U& k)
  {
    return boost::find(m, k) != std::end(m);
  }
}
