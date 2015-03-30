#pragma once

#include <map>
#include <memory>
#include <utility> // std::forward

namespace vcsn
{
  /// Same as std::make_shared, but parameterized by the shared_ptr type,
  /// not the (pointed to) element_type.
  template <typename SharedPtr, typename... Args>
  inline
  SharedPtr
  make_shared_ptr(Args&&... args)
  {
    using type = typename SharedPtr::element_type;
    return std::make_shared<type>(std::forward<Args>(args)...);
  }

  /// Name pointers, to make them easier to read.
  inline
  int address(const void* t)
  {
    static std::map<const void*, int> addresses;
    auto p = addresses.emplace(t, 0);
    if (p.second)
      p.first->second = addresses.size();
    return p.first->second;
  }

  template <typename T>
  inline
  int address(T* t)
  {
    return address((const void*)t);
  }

  template <typename T>
  inline
  int address(const T& t)
  {
    return address(&t);
  }
}
