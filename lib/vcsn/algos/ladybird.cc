#include <vcsn/factory/ladybird.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(ladybird);

    automaton
    ladybird(const context& ctx, unsigned n)
    {
      return details::ladybird_registry().call(ctx->vname(),
                                               ctx, n);
    }
  }
}
