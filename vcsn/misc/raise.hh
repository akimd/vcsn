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
      std::string buf;
      std::getline(is, buf, '\n');
      if (!is.good())
        // This shouldn't really happen; however it's best to fail cleanly.
        is.clear();
      o << str_escape(buf);
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

    /// Serialize std::exception, namely RuntimeErrors.
    //FIXME: I'm sure there's an easier way to do this.
    //Basic parameter overload did not compile,
    //empty template + parameter overload did not either.
    //`long` instead of `int` also doesn't work.
    template <typename T>
    auto print_(std::ostream& o, const T& e, int)
      -> decltype(e.what(), void())
    {
      o << e.what() << "\n";
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
    throw std::runtime_error{o.str()};
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

  /// This value is not starrable.
  template <typename WeightSet>
  ATTRIBUTE_NORETURN
  void raise_not_starrable(const WeightSet& ws,
                           const typename WeightSet::value_t& w)
  {
    raise(ws, ": value is not starrable: ", to_string(ws, w));
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
