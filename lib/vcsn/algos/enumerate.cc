#include <vcsn/algos/enumerate.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(shortest);

    polynomial
    shortest(const automaton& aut, unsigned num)
    {
      return detail::shortest_registry().call(aut, num);
    }

    REGISTER_DEFINE2(enumerate);

    polynomial
    enumerate(const automaton& aut, unsigned max)
    {
      return detail::enumerate_registry().call(aut, max);
    }
  }
}
