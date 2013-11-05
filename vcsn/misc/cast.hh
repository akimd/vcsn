#ifndef VCSN_MISC_CAST_HH
# define VCSN_MISC_CAST_HH

# include <iostream>

# ifdef NDEBUG
#  define down_cast         static_cast
#  define down_pointer_cast std::static_pointer_cast
# else

// We want to support the "down_cast<FOO>(BAR)" syntax (with a pair of
// <> and of ()).  So expand "down_cast" into a templated function.
// Actually, make it a functor that can be given the current location.
#define down_cast                               \
  ::vcsn::detail::down_caster(__FILE__, __LINE__).cast
#define down_pointer_cast down_cast

namespace vcsn
{
  namespace detail
  {
    /// A functor that captures the current location (to report
    /// errors), and provides "cast".
    struct down_caster
    {
      down_caster(const char* f, int l)
        : file(f), line(l)
      {}
      const char* file;
      int line;

      template <typename T, typename U>
      void
      error(const char* msg)
      {
        std::cerr
          << file << ':' << line << ": " << msg
          << " from " << typeid(U).name() << " to " << typeid(T).name()
          << std::endl;
        abort();
      }

      template <typename T, typename U>
      inline
      T
      cast(U t)
      {
        if (!t)
          error<T, U>("down_casting nullptr");
        T res = dynamic_cast<const T>(t);
        if (!res)
          error<T, U>("failed down_cast");
        return res;
      }

      /// Special case for shared_ptr.
      template <typename T, typename U>
      inline
      std::shared_ptr<T>
      cast(std::shared_ptr<U> t)
      {
        if (!t)
          error<std::shared_ptr<T>, std::shared_ptr<U>>("down_casting nullptr");
        std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(t);
        if (!res)
          error<std::shared_ptr<T>, std::shared_ptr<U>>("failed down_cast");
        return res;
      }
    };
  }
}
#endif

#endif // !VCSN_MISC_CAST_HH
