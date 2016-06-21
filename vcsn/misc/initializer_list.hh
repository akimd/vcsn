#pragma once

#include <algorithm>
#include <initializer_list>

namespace vcsn
{

  /// Whether \a e is in \a s.
  template <typename T, typename U>
  inline
  bool
  has(std::initializer_list<T> s, const U& e)
  {
    return std::find(begin(s), end(s), e) != std::end(s);
  }
}
