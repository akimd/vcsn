#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------------.
    | focus(automaton).  |
    `-------------------*/
    REGISTER_DEFINE(focus);
    automaton
    focus(automaton& aut, unsigned tape)
    {
      integral_constant t = {symbol("std::integral_constant<unsigned, "
                                    + std::to_string(tape) + '>')};
      return detail::focus_registry().call(aut, t);
    }
  }
}
