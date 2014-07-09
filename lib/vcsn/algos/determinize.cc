#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(determinize);

    automaton
    determinize(const automaton& aut)
    {
      return detail::determinize_registry().call(aut);
    }


    REGISTER_DEFINE(determinize_weight);

    automaton
    determinize_weight(const automaton& aut)
    {
      return detail::determinize_weight_registry().call(aut);
    }
  }
}
