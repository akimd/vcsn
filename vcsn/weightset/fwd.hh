#ifndef VCSN_WEIGHTSET_FWD_HH
# define VCSN_WEIGHTSET_FWD_HH

namespace vcsn
{
  namespace detail
  {
    // b.hh.
    class b_impl;

    // f2.hh.
    class f2_impl;

    // q.hh.
    class q_impl;

    // qmp.hh.
    class qmp_impl;

    // r.hh.
    class r_impl;

    // rmin.hh.
    class rmin_impl;

    // z.hh.
    class z_impl;

    // zmin.hh.
    class zmin_impl;
  }

  // polynomialset.hh.
  template <class Context>
  class polynomialset;

  // weightset.hh.
  template <typename WeightSet>
  struct variadic_mul_mixin;

  using b    = variadic_mul_mixin<detail::b_impl>;
  using f2   = variadic_mul_mixin<detail::f2_impl>;
  using q    = variadic_mul_mixin<detail::q_impl>;
  using qmp  = variadic_mul_mixin<detail::qmp_impl>;
  using r    = variadic_mul_mixin<detail::r_impl>;
  using rmin = variadic_mul_mixin<detail::rmin_impl>;
  using z    = variadic_mul_mixin<detail::z_impl>;
  using zmin = variadic_mul_mixin<detail::zmin_impl>;

} // namespace vcsn

#endif // !VCSN_WEIGHTSET_FWD_HH
