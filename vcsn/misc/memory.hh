#ifndef VCSN_MISC_MEMORY_HH
# define VCSN_MISC_MEMORY_HH

# include <memory>
# include <utility> // std::forward

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
}

#endif // !VCSN_MISC_MEMORY_HH
