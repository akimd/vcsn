#include <string>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/right-mult.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(right_mult);

    automaton
    right_mult(const automaton& aut, std::string w)
    {
      return detail::right_mult_registry().call(aut->vname(false), aut, w);
    }
  }
}
