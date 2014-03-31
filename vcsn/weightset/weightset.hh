#ifndef VCSN_WEIGHTSETS_WEIGHTSET_HH
# define VCSN_WEIGHTSETS_WEIGHTSET_HH

# include <vcsn/misc/raise.hh> // detail::pass

namespace vcsn
{
  /// Provide a variadic mul on top of a binary mul(), and one().
  template <typename WeightSet>
  struct variadic_mul_mixin : WeightSet
  {
    using super = WeightSet;
    using typename super::value_t;

    // Inherit the constructors.
    using super::super;

    // Provide a variadic mul.
    using super::mul;

    template <typename... Ts>
    value_t mul(const Ts&... ts) const
    {
      value_t res = this->one();
      detail::pass
        {
          (res = super::mul(res, ts))...
        };
      return res;
    }
  };
}

#endif // !VCSN_WEIGHTSETS_WEIGHTSET_HH

