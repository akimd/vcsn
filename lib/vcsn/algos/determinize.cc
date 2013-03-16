#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(determinize);

    automaton
    determinize(const automaton& aut)
    {
      return details::determinize_registry().call(aut->vname(),
                                                  aut);
    }
  }
}
