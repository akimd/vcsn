#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(ladybird);
    automaton
    ladybird(const context& ctx, unsigned n)
    {
      return detail::ladybird_registry().call(ctx, n);
    }
  }
}
