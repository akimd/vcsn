#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/concatenate.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(concatenate);

    automaton
    concatenate(const automaton& lhs, const automaton& rhs)
    {
      return detail::concatenate_registry().call(vname(lhs, rhs), lhs, rhs);
    }

    REGISTER_DEFINE(chain);

    automaton
    chain(const automaton& aut, size_t n)
    {
      return detail::chain_registry().call(aut->vname(false), aut, n);
    }
  }
}
