#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/lift.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {
    /*------------------.
    | lift(automaton).  |
    `------------------*/

    REGISTER_DEFINE(lift_automaton);

    automaton
    lift(const automaton& aut)
    {
      return detail::lift_automaton_registry().call(aut->vname(false),
                                                     aut);
    }

    /*---------------.
    | lift(ratexp).  |
    `---------------*/


    REGISTER_DEFINE(lift_exp);

    ratexp
    lift(const ratexp& e)
    {
      return detail::lift_exp_registry().call(e->ctx().vname(false),
                                               e);
    }
  }
}
