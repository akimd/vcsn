#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------------.
    | is_standard(aut).  |
    `-------------------*/
    REGISTER_DEFINE(is_standard);
    bool
    is_standard(const automaton& a)
    {
      return detail::is_standard_registry().call(a);
    }

    /*---------------------.
    | is_costandard(aut).  |
    `---------------------*/
    REGISTER_DEFINE(is_costandard);
    bool
    is_costandard(const automaton& a)
    {
      return detail::is_costandard_registry().call(a);
    }

    /*----------------.
    | standard(aut).  |
    `----------------*/
    REGISTER_DEFINE(standard);
    automaton
    standard(const automaton& a)
    {
      return detail::standard_registry().call(a);
    }

    /*------------------.
    | costandard(aut).  |
    `------------------*/
    REGISTER_DEFINE(costandard);
    automaton
    costandard(const automaton& a)
    {
      return detail::costandard_registry().call(a);
    }

    /*----------------.
    | standard(exp).  |
    `----------------*/
    REGISTER_DEFINE(standard_expression);
    automaton
    standard(const expression& e)
    {
      return detail::standard_expression_registry().call(e);
    }
  }
}
