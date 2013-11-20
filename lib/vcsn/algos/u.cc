#include <vcsn/factory/u.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    REGISTER_DEFINE(u);

    automaton
    u(const context& ctx, unsigned n)
    {
      return detail::u_registry().call(ctx, n);
    }
  }
}
