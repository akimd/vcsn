#pragma once

#include <iostream>
#include <memory> // std::make_shared

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
    // Is it possible to write a C++ template to check for a
    // function's existence? See vcsn/misc/military-order.hh.
    template <typename>
    struct sfinae_true : std::true_type {};

    template <typename T>
    static auto test_power(int)
      -> sfinae_true<decltype(std::declval<T>()
                              .power(std::declval<typename T::value_t>(),
                                     0))>;

    template <typename>
    static auto test_power(long) -> std::false_type;

    template <typename T>
    struct has_power_member_function
      : decltype(test_power<T>(0))
    {};

    /// Case where the weightset T features a power(value_t, unsigned)
    /// member function.
    template <typename WS>
    auto power_(value_t e, unsigned n) const
      -> vcsn::enable_if_t<has_power_member_function<WS>{}, value_t>
    {
      return super_t::power(e, n);
    }

    /// Case where the weightset T does not feature a
    /// power(value_t, unsigned) member function.
    template <typename WS>
    auto power_(value_t e, unsigned n) const
      -> vcsn::enable_if_t<!has_power_member_function<WS>{}, value_t>
    {
      value_t res = super_t::one();
      if (!super_t::is_one(e))
        while (n-- > 0)
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
}
