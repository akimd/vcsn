#include <vcsn/algos/enumerate.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(shortest);

    std::string
    shortest(const automaton& aut)
    {
      return detail::shortest_registry().call(aut->vname(),
                                              aut);
    }

    REGISTER_DEFINE(enumerate);

    std::vector<std::string>
    enumerate(const automaton& aut, size_t max)
    {
      return detail::enumerate_registry().call(aut->vname(),
                                               aut, max);
    }
  }
}
