#pragma once

#include <deque>

#include <boost/range/algorithm/find.hpp>

#include <vcsn/misc/attributes.hh>

namespace vcsn
{
  /// Whether \a e is member of \a s.
  template <typename T, typename Allocator = std::allocator<T>>
  ATTRIBUTE_PURE
  bool
  has(const std::deque<T, Allocator>& s, const T& e)
  {
    return boost::find(s, e) != std::end(s);
  }
}
