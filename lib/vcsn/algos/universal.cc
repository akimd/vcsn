#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(universal);

    automaton
    universal(const automaton& aut)
    {
      return detail::universal_registry().call(aut);
    }
  }
}
