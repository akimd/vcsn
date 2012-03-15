#ifndef VCSN_MISC_CAST_HH_

# define VCSN_MISC_CAST_HH_
# ifdef NDEBUG
#  define down_cast static_cast
# else

template <typename T, typename U>
inline
T
down_cast(U t)
{
  T res = dynamic_cast<const T>(t);
  assert(res);
  return res;
}

# endif

#endif // !VCSN_MISC_CAST_HH_
