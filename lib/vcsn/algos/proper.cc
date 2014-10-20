#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {
    REGISTER_DEFINE(proper);
    automaton
    proper(const automaton& aut, direction dir, bool prune)
    {
      return detail::proper_registry().call(aut, dir, prune);
    }
  }
}
