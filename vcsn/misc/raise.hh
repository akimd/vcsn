#pragma once

#include <sstream>
#include <stdexcept>
#include <utility>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/to-string.hh> // to_string

namespace vcsn
{
  namespace detail
  {
    /// Ignore its arguments.
    ///
    /// Useful auxiliary to handle variadic arguments.
    struct pass
    {
      template <typename ...T> pass(T...) {}
    };


    /// Serialize arg into o.
    template <typename T>
    void print_(std::ostream& o, const T& arg, long)
    {
      o << arg;
    }

    /// When printing an istream, consider that we are trying to help
    /// the user by showing what's wrong with the input.  So report
    /// some of the upcoming bytes.
    inline void print_(std::ostream& o, std::istream& is, long)
    {
      is.clear();
      auto buf = std::string{};
      std::getline(is, buf, '\n');
      if (!is.good())
        // This shouldn't really happen; however it's best to fail cleanly.
        is.clear();
      o << str_quote(buf);
    }

    /// Disambiguation: both 'istream&' and 'const T&' are elligible.
    inline void print_(std::ostream& o, std::istringstream& is, long)
    {
      print_(o, static_cast<std::istream&>(is), 0);
    }

    /// Serialize arg, which supports print_set, into o.
    template <typename T>
    auto print_(std::ostream& o, const T& arg, int)
      -> decltype(arg.print_set(o), void())
    {
      arg.print_set(o);
    }

    /// Serialize exceptions.
    template <typename T>
    auto print_(std::ostream& o, const T& e, int)
      -> decltype(e.what(), void())
    {
      o << e.what() << '\n';
    }
  }

  /// Raise a runtime_error with the concatenation of \a args as message.
  template <typename... Args>
  ATTRIBUTE_NORETURN
  void raise(Args&&... args)
  {
    std::ostringstream o;
    using swallow = int[];
    (void) swallow
      {
        (detail::print_(o, args, 0), 0)...
      };
    // Remove last \n for exceptions.
    auto msg = o.str();
    if (0 < msg.size() && msg[msg.size() - 1] == '\n')
      msg.pop_back();
    throw std::runtime_error{msg};
  }

  /// If \a b is not verified, raise an error with \a args as message.
  ///
  /// Beware that the arguments are, of course, always evaluated.  So
  /// avoid passing costly function calls.  See VCSN_REQUIRE.
  template <typename Bool, typename... Args>
  void require(Bool b, Args&&... args)
  {
    if (!b)
      raise(std::forward<Args>(args)...);
  }
}

/// A macro similar to require.
///
/// Its point is to avoid the cost of the evaluation of the variadic
/// arguments.
#define VCSN_REQUIRE(Cond, ...)                 \
  do {                                          \
    if (!(Cond))                                \
      ::vcsn::raise(__VA_ARGS__);               \
  } while (false)
