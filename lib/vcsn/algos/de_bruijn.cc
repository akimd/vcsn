#include <vcsn/factory/de_bruijn.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(de_bruijn);

    automaton
    de_bruijn(const context& ctx, unsigned n)
    {
      return details::de_bruijn_registry().call(ctx->vname(),
                                                ctx, n);
    }
  }
}
