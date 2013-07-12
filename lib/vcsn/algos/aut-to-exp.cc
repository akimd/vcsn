#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/aut_to_exp.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {

    /*-------------.
    | aut_to_exp.  |
    `-------------*/

    REGISTER_DEFINE(aut_to_exp);

    ratexp
    aut_to_exp(const automaton& aut)
    {
      return detail::aut_to_exp_registry().call(aut->vname(false),
                                                aut);
    }
  }
}
