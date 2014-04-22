#include <vcsn/factory/random.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(random);

    automaton
    random_automaton(const context& ctx,
                     unsigned num_states, float density,
                     unsigned num_initial, unsigned num_final)
    {
      return detail::random_registry().call(ctx, num_states, density,
                                            num_initial, num_final);
    }


    REGISTER_DEFINE(random_uniform);

    automaton
    random_automaton_uniform(const context& ctx, unsigned num_states)
    {
      return detail::random_uniform_registry().call(ctx, num_states);
    }
  }
}
