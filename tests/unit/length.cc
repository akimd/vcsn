#include <vcsn/misc/string.hh>

// Include this one last, as it defines a macro `V`, which is used as
// a template parameter in boost/unordered/detail/allocate.hpp.
#include "tests/unit/test.hh"

static unsigned
check_length()
{
  unsigned nerrs = 0;
  ASSERT_EQ(0,  vcsn::length(""));
  ASSERT_EQ(1,  vcsn::length("0"));
  ASSERT_EQ(10, vcsn::length("0123456789"));
  ASSERT_EQ(2,  vcsn::length("𝔹𝔹"));
  ASSERT_EQ(8,  vcsn::length("𝔹𝔹 → 𝔹𝔹𝔹"));
  ASSERT_EQ(25, vcsn::length("RatE[[...]? → 𝔹𝔹]  →  𝔹𝔹𝔹"));
  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  nerrs += check_length();
  return !!nerrs;
}
