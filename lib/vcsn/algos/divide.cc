#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/polynomial.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(ldiv_polynomial);
    polynomial
    ldiv(const polynomial& lhs, const polynomial& rhs)
    {
      return detail::ldiv_polynomial_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(lgcd_polynomial);
    polynomial
    lgcd(const polynomial& lhs, const polynomial& rhs)
    {
      return detail::lgcd_polynomial_registry().call(lhs, rhs);
    }
  }
}
