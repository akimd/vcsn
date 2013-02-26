#include <vcsn/factory/ladybird.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    namespace details
    {
      Registry<ladybird_t>&
      ladybird_registry()
      {
        static Registry<ladybird_t> instance{"ladybird"};
        return instance;
      }

      bool ladybird_register(const std::string& ctx, const ladybird_t& fn)
      {
        return ladybird_registry().set(ctx, fn);
      }
    }

    automaton
    ladybird(const context& ctx, unsigned n)
    {
      return details::ladybird_registry().call(ctx->vname(),
                                               ctx, n);
    }
  }
}
