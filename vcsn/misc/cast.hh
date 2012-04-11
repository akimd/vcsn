#ifndef VCSN_MISC_CAST_HH
# define VCSN_MISC_CAST_HH

# ifdef NDEBUG
#  define down_cast static_cast
# else

namespace vcsn
{
  namespace details
  {
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
          << file << ":" << line << ": " << msg
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
    };
  }
}
#define down_cast                               \
  ::vcsn::details::down_caster(__FILE__, __LINE__).cast
#endif

#endif // !VCSN_MISC_CAST_HH
