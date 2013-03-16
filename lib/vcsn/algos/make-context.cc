#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/make-context.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------.
    | make_context.  |
    `---------------*/

    REGISTER_DEFINE(make_context);

    context
    make_context(const std::string& name)
    {
      return details::make_context_registry()
        .call(abstract_context::sname(name), name);
    }


    /*-----------------.
    | make_ratexpset.  |
    `-----------------*/

    REGISTER_DEFINE(make_ratexpset);

    ratexpset
    make_ratexpset(const context& ctx)
    {
      return details::make_ratexpset_registry().call(ctx->vname(), ctx);
    }
  }
}
