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
      return detail::is_standard_registry().call(a->vname(false), a);
    }

    /*----------------.
    | standard(aut).  |
    `----------------*/
    REGISTER_DEFINE(standard);

    automaton
    standard(const automaton& a)
    {
      return detail::standard_registry().call(a->vname(false), a);
    }

    /*----------------.
    | standard(exp).  |
    `----------------*/
    REGISTER_DEFINE(standard_exp);

    automaton
    standard(const ratexp& e)
    {
      return detail::standard_exp_registry().call(e->vname(false), e);
    }
  }
}
