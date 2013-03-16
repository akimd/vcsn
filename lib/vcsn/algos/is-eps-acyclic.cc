#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*----------------.
  | is_eps_acyclic. |
  `----------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_eps_acyclic);

    bool is_eps_acyclic(const automaton& aut)
    {
      return details::is_eps_acyclic_registry().call(aut->vname(), aut);
    }
  }
}
