#pragma once

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace detail
  {
    /// Greatest common divisor.
    ATTRIBUTE_PURE
    inline
    unsigned int gcd(unsigned int a, unsigned int b)
    {
      require(b, "gcd: rhs cannot be zero");
      while (b)
      {
        unsigned int t = a;
        a = b;
        b = t % b;
      }
      return a;
    }

    /// Lowest common multiple
    ATTRIBUTE_PURE
    inline
    unsigned int lcm(unsigned int a, unsigned int b)
    {
      return a / gcd(a, b) * b;
    }
  }
}
