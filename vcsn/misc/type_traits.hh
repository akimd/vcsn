#pragma once

namespace vcsn
{
  // Wait for C++14.
  template <bool Cond, typename T = void>
  using enable_if_t = typename std::enable_if<Cond, T>::type;
}
