#ifndef VCSN_MISC_ATTRIBUTES_HH
# define VCSN_MISC_ATTRIBUTES_HH

# if defined __clang__

#  define ATTRIBUTE_NORETURN [[noreturn]]
#  define ATTRIBUTE_PURE __attribute__((pure))

# elif defined __GNUC__

#  define ATTRIBUTE_NORETURN __attribute__((noreturn))
#  define ATTRIBUTE_PURE __attribute__((pure))

# elif defined _MSC_VER

#  define ATTRIBUTE_NORETURN [[noreturn]]
#  define ATTRIBUTE_PURE

# endif


#endif // !VCSN_MISC_ATTRIBUTES_HH
