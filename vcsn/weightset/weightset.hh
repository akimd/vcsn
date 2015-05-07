#pragma once

#include <iostream>
#include <memory> // std::make_shared

// It is much simpler and saner in C++ to put types and functions on
// these types in the same namespace.  Since "using q =
// detail::variadic_mul_mixin<q_impl>" would just create an alias of
// q, its original namespace, detail::, would still be the namespace
// used in ADL.
//
// This is really troublesome to implement free-functions such as join.
//
// Therefore, although this wrapper should be hidden as a detail::, it
// will remain in vcsn::, where join and the like will find it.

namespace vcsn
{
  /// Provide a variadic mul on top of a binary mul(), and one().
  template <typename WeightSet>
  struct variadic_mul_mixin : WeightSet
  {
    using super_t = WeightSet;
    using typename super_t::value_t;

    /// Inherit the constructors.
    using super_t::super_t;

    /// Import mul overloads.
    using super_t::mul;

    /// A variadic multiplication.
    template <typename... Ts>
    value_t mul(const Ts&... ts) const
    {
      value_t res = this->one();
      // FIXME: Remove once GCC is fixed.
      using swallow = int[];
      (void) swallow
        {
          ((res = super_t::mul(res, ts)), 0)...
        };
      return res;
    }

    /// Repeated multiplication.
    value_t power(value_t e, unsigned n) const
    {
      value_t res = super_t::one();
      if (!super_t::is_one(e))
        while (n-- > 0)
          res = mul(res, e);
      return res;
    }
  };
}
