#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/make-context.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------.
    | make_context.  |
    `---------------*/

    namespace details
    {

      Registry<make_context_t>&
      make_context_registry()
      {
        static Registry<make_context_t> instance{"make_context"};
        return instance;
      }

      bool
      make_context_register(const std::string& ctx, const make_context_t& fn)
      {
        return make_context_registry().set(ctx, fn);
      }
    }

    context*
    make_context(const std::string& name, const std::string& gens)
    {
      return details::make_context_registry().call(name, gens);
    }


    /*------------------.
    | make_kratexpset.  |
    `------------------*/

    namespace details
    {

      Registry<make_kratexpset_t>&
      make_kratexpset_registry()
      {
        static Registry<make_kratexpset_t> instance{"make_kratexpset"};
        return instance;
      }

      bool
      make_kratexpset_register(const std::string& ctx,
                               const make_kratexpset_t& fn)
      {
        return make_kratexpset_registry().set(ctx, fn);
      }
    }

    abstract_kratexpset*
    make_kratexpset(const context& ctx)
    {
      return details::make_kratexpset_registry().call(ctx.vname(), ctx);
    }
  }
}
