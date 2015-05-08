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
    return in(k, m);
  }

  /// Whether \a e is member of \a m.
  template <typename T, typename U>
  inline
  bool
  in(const U& k, const std::initializer_list<T>& m)
  {
    return boost::find(m, k) != std::end(m);
  }

  /// Whether \a e is member of \a m.
  template <typename T, typename U>
  inline
  bool
  operator%(const U& k, const std::initializer_list<T>& m)
  {
    return in(k, m);
  }
}
