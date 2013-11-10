#ifndef LIB_VCSN_DOT_SCAN_HH
# define LIB_VCSN_DOT_SCAN_HH

// Set parameters for Flex header, and include it.
# define YY_FLEX_NAMESPACE_BEGIN                \
  namespace vcsn {                              \
    namespace detail {                          \
      namespace dot {

# define YY_FLEX_NAMESPACE_END                  \
      }                                         \
    }                                           \
  }

# include <vcsn/misc/flex-lexer.hh>

#endif // !LIB_VCSN_DOT_SCAN_HH
