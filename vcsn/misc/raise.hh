#pragma once

#include <stdexcept>
#include <sstream>
#include <utility>

#include <vcsn/misc/attributes.hh>

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


    /// Serialize arg into o.
    template <typename T>
    void print_(std::ostream& o, const T& arg, long)
    {
      o << arg;
    }

    /// Serialize arg, which supports print_set, into o.
    template <typename T>
    auto print_(std::ostream& o, const T& arg, int)
      -> decltype(arg.print_set(o), void())
    {
      arg.print_set(o);
    }
  }

  /// Raise a runtime_error with the concatenation of \a args as message.
  template <typename... Args>
  ATTRIBUTE_NORETURN
  inline void raise(Args&&... args)
  {
    std::ostringstream o;
    using swallow = int[];
    (void) swallow
      {
        (detail::print_(o, args, 0), 0)...
      };
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
