#include <vector>

#include <vcsn/factory/double-ring.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(double_ring);

    automaton
    double_ring(const context& ctx, unsigned n, std::vector<unsigned> f)
    {
      return detail::double_ring_registry().call(ctx->vname(false),
                                               ctx, n, f);
    }
  }
}
