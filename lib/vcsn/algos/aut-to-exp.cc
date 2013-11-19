#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/aut-to-exp.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {

    /*------------------.
    | eliminate_state.  |
    `------------------*/

    REGISTER_DEFINE2(eliminate_state);

    automaton
    eliminate_state(const automaton& aut, int i)
    {
      return detail::eliminate_state_registry().call(aut, i);
    }

    /*-------------.
    | aut_to_exp.  |
    `-------------*/

    REGISTER_DEFINE(aut_to_exp);

    ratexp
    aut_to_exp(const automaton& aut)
    {
      return detail::aut_to_exp_registry().call(aut);
    }
  }
}
