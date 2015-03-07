#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(random_automaton);
    automaton
    random_automaton(const context& ctx,
                     unsigned num_states, float density,
                     unsigned num_initial, unsigned num_final)
    {
      return detail::random_automaton_registry().call(ctx, num_states, density,
                                                      num_initial, num_final);
    }

    REGISTER_DEFINE(random_automaton_deterministic);
    automaton
    random_automaton_deterministic(const context& ctx, unsigned num_states)
    {
      return detail::random_automaton_deterministic_registry().call(ctx, num_states);
    }
  }
}
