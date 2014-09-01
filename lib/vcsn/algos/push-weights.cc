#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/push-weights.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*--------------.
  | push weight.  |
  `--------------*/

  namespace dyn
  {
    REGISTER_DEFINE(push_weights);

    automaton
    push_weights(const automaton& aut)
    {
      return detail::push_weights_registry().call(aut);
    }
  }
}
