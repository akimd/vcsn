#include <vector>

#include <vcsn/algos/double-ring.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(double_ring);

    automaton
    double_ring(const context& ctx, unsigned n, const std::vector<unsigned>& f)
    {
      return detail::double_ring_registry().call(ctx, n, f);
    }
  }
}
