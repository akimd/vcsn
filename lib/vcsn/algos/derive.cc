#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/derive.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*----------------------.
    | derive(exp, string).  |
    `----------------------*/
    REGISTER_DEFINE(derive);

    polynomial
    derive(const ratexp& e, const std::string& s)
    {
      return detail::derive_registry().call(e->vname(false), e, s);
    }
  }
}
