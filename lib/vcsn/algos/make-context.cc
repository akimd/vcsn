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

    context
    make_context(const std::string& name)
    {
      return details::make_context_registry()
        .call(abstract_context::sname(name), name);
    }


    /*-----------------.
    | make_ratexpset.  |
    `-----------------*/

    namespace details
    {

      Registry<make_ratexpset_t>&
      make_ratexpset_registry()
      {
        static Registry<make_ratexpset_t> instance{"make_ratexpset"};
        return instance;
      }

      bool
      make_ratexpset_register(const std::string& ctx,
                              const make_ratexpset_t& fn)
      {
        return make_ratexpset_registry().set(ctx, fn);
      }
    }

    ratexpset
    make_ratexpset(const context& ctx)
    {
      return details::make_ratexpset_registry().call(ctx->vname(), ctx);
    }
  }
}
