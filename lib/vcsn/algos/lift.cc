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
      return detail::lift_automaton_registry().call(aut);
    }

    /*---------------.
    | lift(ratexp).  |
    `---------------*/


    REGISTER_DEFINE(lift_ratexp);

    ratexp
    lift(const ratexp& e)
    {
      return detail::lift_ratexp_registry().call(e);
    }
  }
}
