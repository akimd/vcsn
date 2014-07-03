#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/blind.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------------.
    | blind(automaton).  |
    `-------------------*/
    REGISTER_DEFINE(blind);
    automaton
    blind(automaton& aut, unsigned tape)
    {
      return detail::blind_registry().call(aut, tape);
    }
  }
}
