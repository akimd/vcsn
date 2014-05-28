#include <vcsn/algos/strip.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(strip);
    automaton
    strip(const automaton& aut)
    {
      return detail::strip_registry().call(aut);
    }
  }
}
