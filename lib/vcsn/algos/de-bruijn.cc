#include <vcsn/factory/de-bruijn.hh>
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
      return detail::de_bruijn_registry().call(ctx, n);
    }
  }
}
