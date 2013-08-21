#include <string>

#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/left-mult.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(left_mult);

    automaton
    left_mult(const automaton& aut, std::string w)
    {
      return detail::left_mult_registry().call(aut->vname(false), aut, w);
    }
  }
}
