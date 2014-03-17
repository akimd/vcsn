#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

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

    /*------------------------------.
    | copy(ratexp, out_ratexpset).  |
    `------------------------------*/

    REGISTER_DEFINE(copy_ratexp);

    ratexp
    copy(const ratexp& e, const ratexpset& rs)
    {
      return detail::copy_ratexp_registry().call(e, rs);
    }
  }
}
