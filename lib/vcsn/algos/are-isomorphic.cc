#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/are-isomorphic.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(are_isomorphic);

    bool
    are_isomorphic(const automaton& lhs, const automaton& rhs)
    {
      return detail::are_isomorphic_registry().call(lhs, rhs);
    }

    /* FIXME: I could add an overloaded version working on ratexps,
       but it seems really pointless in this case. */
  }
}
