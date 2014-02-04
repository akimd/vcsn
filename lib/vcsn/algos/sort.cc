#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/sort.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*------.
  | sort. |
  `------*/

  namespace dyn
  {
    REGISTER_DEFINE(sort);

    automaton
    sort(const automaton& aut)
    {
      return detail::sort_registry().call(aut);
    }
  }
}
