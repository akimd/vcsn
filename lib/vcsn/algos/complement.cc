#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/complement.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(complement);

    automaton
    complement(const automaton& aut)
    {
      return detail::complement_registry().call(aut);
    }
  }
}
