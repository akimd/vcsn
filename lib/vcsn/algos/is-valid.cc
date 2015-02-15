#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*----------------------.
    | is_valid(automaton).  |
    `----------------------*/

    REGISTER_DEFINE(is_valid);
    bool
    is_valid(const automaton& a)
    {
      return detail::is_valid_registry().call(a);
    }

    /*------------------------.
    | is_valid(expression).   |
    `------------------------*/

    REGISTER_DEFINE(is_valid_expression);
    bool
    is_valid(const expression& e)
    {
      return detail::is_valid_expression_registry().call(e);
    }
  }
}
