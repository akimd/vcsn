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


    /*--------------------.
    | transpose(ratexp).  |
    `--------------------*/

    REGISTER_DEFINE(transpose_ratexp);

    ratexp
    transpose(const dyn::ratexp& e)
    {
      return detail::transpose_ratexp_registry().call(e);
    }

    REGISTER_DEFINE(transposition_ratexp);
    ratexp
    transposition(const ratexp& r)
    {
      return detail::transposition_ratexp_registry().call(r);
    }
  }
}
