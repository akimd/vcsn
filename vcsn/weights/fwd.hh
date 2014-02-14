#ifndef VCSN_WEIGHTS_FWD_HH
# define VCSN_WEIGHTS_FWD_HH

namespace vcsn
{

  // b.hh.
  class b;

  // f2.hh.
  class f2;

  // polynomialset.hh.
  template <class Context>
  class polynomialset;

  // q.hh.
  class q;

  // r.hh.
  class r;

  // z.hh.
  class z;

  // zmin.hh.
  class zmin;

} // namespace vcsn

#define VCSN_WEIGHTS_BINARY(Lhs, Rhs, Res)      \
  /** The join of two weightsets. */            \
  inline                                        \
  Res join(const Lhs&, const Rhs&)              \
  {                                             \
    return {};                                  \
  }

#endif // !VCSN_WEIGHTS_FWD_HH
