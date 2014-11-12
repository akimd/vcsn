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
    | lift(expression).  |
    `---------------*/


    REGISTER_DEFINE(lift_expression);

    expression
    lift(const expression& e)
    {
      return detail::lift_expression_registry().call(e);
    }
  }
}
