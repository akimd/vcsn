#pragma once

#include <iostream>
#include <memory> // std::make_shared

#include <vcsn/misc/type_traits.hh> // detect

// It is much simpler and saner in C++ to put types and functions on
// these types in the same namespace.  Since "using q =
// detail::weightset_mixin<q_impl>" would just create an alias of
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
  struct weightset_mixin : WeightSet
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

  private:
    /// The signature of power.
    template <typename T>
    using power_t = decltype(std::declval<T>()
                             .power(std::declval<typename T::value_t>(), 0));

    /// Whether T features a power member function.
    template <typename T>
    using has_power_mem_fn = detail::detect<T, power_t>;

    /// Case where the weightset T features a power(value_t, unsigned)
    /// member function.
    template <typename WS = super_t>
    auto power_(value_t e, unsigned n) const
      -> std::enable_if_t<has_power_mem_fn<WS>{}, value_t>
    {
      return super_t::power(e, n);
    }

    /// Case where the weightset T does not feature a
    /// power(value_t, unsigned) member function.
    template <typename WS = super_t>
    auto power_(value_t e, unsigned n) const
      -> std::enable_if_t<!has_power_mem_fn<WS>{}, value_t>
    {
      value_t res = super_t::one();
      if (!super_t::is_one(e))
        while (n--)
          res = mul(res, e);
      return res;
    }

  public:

    /// Repeated multiplication.
    value_t power(value_t e, unsigned n) const
    {
      return power_<WeightSet>(e, n);
    }
  };

  // FIXME: find generic implementation for min-plus.
  template <typename T>
  struct is_tropical : std::false_type
  {};

}
