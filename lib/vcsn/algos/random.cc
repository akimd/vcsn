#include <vcsn/factory/random.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(random);

    automaton
    random_automaton(const context& ctx, unsigned num_states)
    {
      return detail::random_registry().call(ctx->vname(false),
                                            ctx, num_states);
    }
  }
}
