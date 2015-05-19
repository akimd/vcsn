#pragma once

#if defined __clang__ || defined __GNUC__

# define BUILTIN_UNREACHABLE() __builtin_unreachable()

#elif defined _MSC_VER

# define BUILTIN_UNREACHABLE() __assume(0)

#else

# define BUILTIN_UNREACHABLE()                  \
  do {                                          \
    assert(!"unreachable code");                \
    abort();                                    \
  } while (false)

#endif
