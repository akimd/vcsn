#ifndef VCSN_MISC_RAISE_HH
# define VCSN_MISC_RAISE_HH

# include <stdexcept>
# include <sstream>
# include <utility>

# include <vcsn/misc/attributes.hh>

namespace vcsn
{
  namespace detail
  {
    /// Ignore its arguments.
    ///
    /// Useful auxiliary to handle variadic arguments.
    struct pass
    {
      template<typename ...T> pass(T...) {}
    };
  }

  /// Raise a runtime_error with the concatenation of \a args as message.
  template <typename... Args>
  ATTRIBUTE_NORETURN
  inline void raise(const Args&... args)
  {
    std::ostringstream o;
    detail::pass{(o << args, 0)...};
    throw std::runtime_error{o.str()};
  }

  /// If \a b is not verified, raise an error with \a args as message.
  template <typename... Args>
  inline void require(bool b, Args&&... args)
  {
    if (!b)
      raise(std::forward<Args>(args)...);
  }
};

#endif // !VCSN_MISC_RAISE_HH
