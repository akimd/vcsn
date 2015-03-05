#pragma once

#if defined __clang__

# define ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
# define ATTRIBUTE_CONST         __attribute__((const))
# define ATTRIBUTE_DLLEXPORT
# define ATTRIBUTE_DLLIMPORT
# define ATTRIBUTE_HOT           __attribute__((hot))
# define ATTRIBUTE_NORETURN      [[noreturn]]
# define ATTRIBUTE_PURE          __attribute__((pure))

#elif defined __GNUC__

# define ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
# define ATTRIBUTE_CONST         __attribute__((const))
# define ATTRIBUTE_DLLEXPORT     __attribute__((visibility("default")))
# define ATTRIBUTE_DLLIMPORT     ATTRIBUTE_DLLEXPORT
# define ATTRIBUTE_HOT           __attribute__((hot))
# define ATTRIBUTE_NORETURN      __attribute__((noreturn))
# define ATTRIBUTE_PURE          __attribute__((pure))

#elif defined _MSC_VER

# define ATTRIBUTE_ALWAYS_INLINE
# define ATTRIBUTE_CONST         __declspec(const)
# define ATTRIBUTE_DLLEXPORT     __declspec(dllexport)
# define ATTRIBUTE_DLLIMPORT     __declspec(dllimport)
# define ATTRIBUTE_HOT
# define ATTRIBUTE_NORETURN      [[noreturn]]
# define ATTRIBUTE_PURE

#endif
