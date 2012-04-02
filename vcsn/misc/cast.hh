#ifndef VCSN_MISC_CAST_HH_

# define VCSN_MISC_CAST_HH_
# ifdef NDEBUG
#  define down_cast static_cast
#  define maybe_down_cast static_cast

# else

template <typename T, typename U>
inline
T
maybe_down_cast(U t)
{
  return dynamic_cast<const T>(t);
}

struct down_caster
{
  down_caster(const char* f, int l)
    : file(f), line(l)
  {}
  const char* file;
  int line;

  template <typename T, typename U>
  inline
  T
  cast(U t)
  {
    T res = maybe_down_cast<T>(t);
    if (!res)
      {
        std::cerr << file << ":" << line << ": failed down_cast" << std::endl;
        abort();
      }
    return res;
  }
};

#define down_cast                               \
  down_caster(__FILE__, __LINE__).cast
#endif

#endif // !VCSN_MISC_CAST_HH_
