#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(shortest);
    polynomial
    shortest(const automaton& aut, unsigned num)
    {
      return detail::shortest_registry().call(aut, num);
    }

    REGISTER_DEFINE(enumerate);
    polynomial
    enumerate(const automaton& aut, unsigned max)
    {
      return detail::enumerate_registry().call(aut, max);
    }
  }
}
