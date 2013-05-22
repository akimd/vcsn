#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/fsm.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------.
    | fsm.  |
    `------*/

    REGISTER_DEFINE(fsm);

    std::ostream&
    fsm(const automaton& aut, std::ostream& out)
    {
      detail::fsm_registry().call(aut->vname(),
                                  aut, out);
      return out;
    }
  }
}
