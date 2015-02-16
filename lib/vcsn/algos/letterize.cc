#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(letterize);

    automaton
    letterize(const automaton& aut)
    {
      return detail::letterize_registry().call(aut);
    }
  }
}
