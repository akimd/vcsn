#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/prefix.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*--------.
  | prefix.  |
  `-----------*/

  namespace dyn
  {
    REGISTER_DEFINE(prefix);

    automaton
    prefix(const automaton& aut)
    {
      return detail::prefix_registry().call(aut);
    }
  }
}
