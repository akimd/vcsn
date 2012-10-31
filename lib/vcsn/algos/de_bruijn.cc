#include <vcsn/factory/de_bruijn.hh>
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    namespace details
    {
      Registry<de_bruijn_t>&
      de_bruijn_registry()
      {
        static Registry<de_bruijn_t> instance{"de_bruijn"};
        return instance;
      }

      bool de_bruijn_register(const std::string& ctx, const de_bruijn_t& fn)
      {
        return de_bruijn_registry().set(ctx, fn);
      }
    }

    automaton
    de_bruijn(const context& ctx, unsigned n)
    {
      return details::de_bruijn_registry().call(ctx.vname(),
                                                ctx, n);
    }
  }
}
