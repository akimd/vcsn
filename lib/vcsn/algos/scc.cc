#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/scc.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*-----.
  | SCC. |
  `-----*/

  namespace dyn
  {

    REGISTER_DEFINE(num_sccs);

    int
    num_sccs(const automaton& aut)
    {
      return detail::num_sccs_registry().call(aut);
    }
  }
}
