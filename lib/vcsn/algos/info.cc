#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*------------------.
    | info(automaton).  |
    `------------------*/

    REGISTER_DEFINE(info);
    std::ostream&
    info(const automaton& aut, std::ostream& out, bool detailed)
    {
      detail::info_registry().call(aut, out, detailed);
      return out;
    }

    /*--------------------.
    | info(expression).   |
    `--------------------*/

    REGISTER_DEFINE(info_expression);
    std::ostream&
    info(const dyn::expression& e, std::ostream& out)
    {
      detail::info_expression_registry().call(e, out);
      return out;
    }
  }
}
