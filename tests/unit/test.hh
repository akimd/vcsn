#ifndef VCSN_TESTS_UNIT_TEST_HH
# define VCSN_TESTS_UNIT_TEST_HH

# include <iostream>

# include <vcsn/misc/echo.hh>

# define ASSERT_EQ(Lhs, Rhs)                                    \
  do {                                                          \
    auto lhs = Lhs;                                             \
    auto rhs = Rhs;                                             \
    if (lhs != rhs)                                             \
      {                                                         \
        SHOWH("assertion failed: " #Lhs " == " #Rhs);           \
        SHOWH("    " << lhs << " != " << rhs);                  \
        res = false;                                            \
      }                                                         \
  } while (false)

#endif // !VCSN_TESTS_UNIT_TEST_HH

