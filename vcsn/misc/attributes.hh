#ifndef VCSN_MISC_ATTRIBUTES_HH
# define VCSN_MISC_ATTRIBUTES_HH

# if defined __clang__

#  define ATTRIBUTE_PURE
#  define ATTRIBUTE_NORETURN [[noreturn]]

# elif defined __GNUC__

#  define ATTRIBUTE_PURE __attribute__((pure))
#  define ATTRIBUTE_NORETURN __attribute__((noreturn))

# elif defined _MSC_VER

#  define ATTRIBUTE_PURE
#  define ATTRIBUTE_NORETURN [[noreturn]]

# endif


#endif // !VCSN_MISC_ATTRIBUTES_HH
