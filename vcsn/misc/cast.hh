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

template <typename T, typename U>
inline
T
down_cast(U t)
{
  T res = maybe_down_cast<T>(t);
  assert(res);
  return res;
}

# endif

#endif // !VCSN_MISC_CAST_HH_
