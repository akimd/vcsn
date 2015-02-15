#include <vector>

#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
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
