#ifndef VCSN_WEIGHTSETS_FWD_HH
# define VCSN_WEIGHTSETS_FWD_HH

namespace vcsn
{

  // b.hh.
  class b_impl;

  // f2.hh.
  class f2_impl;

  // polynomialset.hh.
  template <class Context>
  class polynomialset;

  // q.hh.
  class q_impl;

  // r.hh.
  class r_impl;

  // z.hh.
  class z_impl;

  // zmin.hh.
  class zmin_impl;

  // weightset.hh.
  template <typename WeightSet>
  struct variadic_mul_mixin;

  using b = variadic_mul_mixin<b_impl>;
  using z = variadic_mul_mixin<z_impl>;
  using q = variadic_mul_mixin<q_impl>;
  using r = variadic_mul_mixin<r_impl>;

  using zmin = variadic_mul_mixin<zmin_impl>;
  using f2 = variadic_mul_mixin<f2_impl>;

} // namespace vcsn

#define VCSN_WEIGHTS_BINARY(Lhs, Rhs, Res)      \
  /** The join of two weightsets. */            \
  inline                                        \
  Res join(const Lhs&, const Rhs&)              \
  {                                             \
    return {};                                  \
  }

#endif // !VCSN_WEIGHTSETS_FWD_HH
