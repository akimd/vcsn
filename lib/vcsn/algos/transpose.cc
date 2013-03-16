#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/transpose.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------------------.
    | transpose for automata.  |
    `-------------------------*/

    REGISTER_DEFINE(transpose);

    automaton
    transpose(automaton& aut)
    {
      return details::transpose_registry().call(aut->vname(),
                                                aut);
    }


    /*-----------------------.
    | transpose for ratexp.  |
    `-----------------------*/

    REGISTER_DEFINE(transpose_exp);

    ratexp
    transpose(const dyn::ratexp& e)
    {
      return details::transpose_exp_registry().call(e->ctx().vname(),
                                                    e);
    }
  }
}
