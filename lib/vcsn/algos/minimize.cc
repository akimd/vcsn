#include <vcsn/algos/minimize.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(minimize);

    automaton
    minimize(const automaton& aut)
    {
      return detail::minimize_registry().call(aut->vname(false), aut);
    }
  }
}
