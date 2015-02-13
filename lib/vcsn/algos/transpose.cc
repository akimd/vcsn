#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-----------------------.
    | transpose(automaton).  |
    `-----------------------*/

    REGISTER_DEFINE(transpose);
    automaton
    transpose(automaton& aut)
    {
      return detail::transpose_registry().call(aut);
    }


    /*-------------------------.
    | transpose(expression).   |
    `-------------------------*/

    REGISTER_DEFINE(transpose_expression);
    expression
    transpose(const dyn::expression& e)
    {
      return detail::transpose_expression_registry().call(e);
    }

    REGISTER_DEFINE(transposition_expression);
    expression
    transposition(const expression& r)
    {
      return detail::transposition_expression_registry().call(r);
    }
  }
}
