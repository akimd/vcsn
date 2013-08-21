#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/chain.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(chain);

    automaton
    chain(const automaton& aut, size_t n)
    {
      return detail::chain_registry().call(aut->vname(false), aut, n);
    }
  }
}
