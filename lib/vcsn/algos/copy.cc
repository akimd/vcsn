#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/expressionset.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------------------.
    | copy(automaton).  |
    `------------------*/

    REGISTER_DEFINE(copy);
    automaton
    copy(const automaton& aut)
    {
      return detail::copy_registry().call(aut);
    }

    REGISTER_DEFINE(copy_convert);
    automaton
    copy(const automaton& aut, const context& ctx)
    {
      return detail::copy_convert_registry().call(aut, ctx);
    }

    /*---------------------------------------.
    | copy(expression, out_expressionset).   |
    `---------------------------------------*/

    REGISTER_DEFINE(copy_expression);
    expression
    copy(const expression& e, const expressionset& rs)
    {
      return detail::copy_expression_registry().call(e, rs);
    }
  }
}
