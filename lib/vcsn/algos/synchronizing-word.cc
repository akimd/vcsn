#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/synchronizing-word.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(pair);
    automaton
    pair(const automaton& aut)
    {
      return detail::pair_registry().call(aut);
    }
  }
}
