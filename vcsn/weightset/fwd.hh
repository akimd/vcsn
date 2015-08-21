#pragma once

#include <vcsn/misc/wet.hh> // wet_kind

namespace vcsn
{
  namespace detail
  {
    // b.hh.
    class b_impl;

    // f2.hh.
    class f2_impl;

    // log.hh
    class log_impl;

    // nmin.hh.
    class nmin_impl;

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

    // polynomialset.hh.
    template <typename Context, wet_kind_t Kind>
    class polynomialset_impl;
  }

  // weightset.hh.
  template <typename WeightSet>
  struct weightset_mixin;

  using b    = weightset_mixin<detail::b_impl>;
  using f2   = weightset_mixin<detail::f2_impl>;
  using log  = weightset_mixin<detail::log_impl>;
  using nmin = weightset_mixin<detail::nmin_impl>;
  using q    = weightset_mixin<detail::q_impl>;
  using qmp  = weightset_mixin<detail::qmp_impl>;
  using r    = weightset_mixin<detail::r_impl>;
  using rmin = weightset_mixin<detail::rmin_impl>;
  using z    = weightset_mixin<detail::z_impl>;
  using zmin = weightset_mixin<detail::zmin_impl>;

  template <typename Context,
            wet_kind_t Kind = detail::wet_kind<labelset_t_of<Context>,
                                               weightset_t_of<Context>>()>
  using polynomialset
    = weightset_mixin<detail::polynomialset_impl<Context, Kind>>;
} // namespace vcsn
