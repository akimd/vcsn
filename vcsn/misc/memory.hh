#pragma once

#include <map>
#include <memory>
#include <utility> // std::forward

namespace vcsn
{
  /// Same as std::make_shared, but parameterized by the shared_ptr type,
  /// not the (pointed to) element_type.
  template <typename SharedPtr, typename... Args>
  SharedPtr
  make_shared_ptr(Args&&... args)
  {
    using type = typename SharedPtr::element_type;
    return std::make_shared<type>(std::forward<Args>(args)...);
  }

  /// Name pointers, to make them easier to read.
  inline
  unsigned address(const void* t)
  {
    static auto addresses = std::map<const void*, unsigned>{};
    auto p = addresses.emplace(t, 0);
    if (p.second)
      p.first->second = addresses.size();
    return p.first->second;
  }

  template <typename T>
  unsigned address(T* t)
  {
    return address((const void*)t);
  }

  template <typename T>
  unsigned address(const T& t)
  {
    return address(&t);
  }
}
