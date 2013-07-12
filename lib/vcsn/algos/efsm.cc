#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/efsm.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------.
    | efsm.  |
    `-------*/

    REGISTER_DEFINE(efsm);

    std::ostream&
    efsm(const automaton& aut, std::ostream& out)
    {
      detail::efsm_registry().call(aut->vname(false),
                                  aut, out);
      return out;
    }
  }
}
