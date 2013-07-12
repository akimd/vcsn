#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(determinize);

    automaton
    determinize(const automaton& aut)
    {
      return detail::determinize_registry().call(aut->vname(false),
                                                  aut);
    }
  }
}
