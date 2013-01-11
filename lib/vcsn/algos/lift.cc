#include <vcsn/algos/dyn.hh>
#include <vcsn/algos/lift.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {
    /*------------------.
    | lift(automaton).  |
    `------------------*/

    namespace details
    {
      Registry<lift_automaton_t>&
      lift_automaton_registry()
      {
        static Registry<lift_automaton_t> instance{"lift"};
        return instance;
      }

      bool lift_automaton_register(const std::string& ctx,
                                   const lift_automaton_t& fn)
      {
        return lift_automaton_registry().set(ctx, fn);
      }
    }

    automaton
    lift(const automaton& aut)
    {
      return details::lift_automaton_registry().call(aut->vname(),
                                                     aut);
    }
    }
  }
}
