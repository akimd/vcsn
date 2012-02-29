#ifndef VCSN_MISC_CAST_HH_

# define VCSN_MISC_CAST_HH_
# ifdef NDEBUG
#  define down_cast static_cast
# else
#  define down_cast dynamic_cast
# endif

#endif // !VCSN_MISC_CAST_HH_
