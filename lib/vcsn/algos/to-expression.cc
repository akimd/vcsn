#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/to-expression.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {

    /*------------------.
    | eliminate_state.  |
    `------------------*/

    REGISTER_DEFINE(eliminate_state);

    automaton
    eliminate_state(const automaton& aut, int i)
    {
      return detail::eliminate_state_registry().call(aut, i);
    }

    /*-----------------.
    | to_expression.   |
    `-----------------*/

    REGISTER_DEFINE(to_expression);

    ratexp
    to_expression(const automaton& aut, const std::string& algo)
    {
      return detail::to_expression_registry().call(aut, algo);
    }
  }
}
