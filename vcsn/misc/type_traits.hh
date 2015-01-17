#pragma once

#include <type_traits>

namespace vcsn
{
  // Wait for C++14.  GCC 4.8 does not yet have std::enable_if_t and
  // the like.
  template <bool Cond, typename T = void>
  using enable_if_t = typename std::enable_if<Cond, T>::type;

  template <typename T, typename U>
  using is_same_t = typename std::is_same<T, U>::type;
}
