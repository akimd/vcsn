#ifndef VCSN_MISC_ATTRIBUTES_HH
# define VCSN_MISC_ATTRIBUTES_HH

# ifdef __clang__

#  define ATTRIBUTE_PURE
#  define ATTRIBUTE_NORETURN [[noreturn]]

# elif defined(__GNUC__)

#  define ATTRIBUTE_PURE __attribute((pure))
#  define ATTRIBUTE_NORETURN __attribute((noreturn))

# elif defined(_MSC_VER)

#  define ATTRIBUTE_PURE
#  define ATTRIBUTE_NORETURN [[noreturn]]

# endif


#endif // !VCSN_MISC_ATTRIBUTES_HH
