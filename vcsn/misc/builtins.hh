#ifndef VCSN_MISC_BUILTINS_HH
# define VCSN_MISC_BUILTINS_HH

# if defined __clang__ || defined __GNU__

#  define BUILTIN_UNREACHABLE() __builtin_unreachable()

# elif defined _MSC_VER

#  define BUILTIN_UNREACHABLE() __assume(0)

# else

#  define BUILTIN_UNREACHABLE()                 \
  do {                                          \
    assert(!"unreachable code");		\
    abort();					\
  } while (false)

# endif

#endif // !VCSN_MISC_BUILTINS_HH
