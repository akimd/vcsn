#include <vcsn/dyn/algos.hh>
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
      return detail::make_context_registry()
        .call(detail::abstract_context::sname(name), name);
    }


    /*-----------------.
    | make_ratexpset.  |
    `-----------------*/

    REGISTER_DEFINE(make_ratexpset);

    ratexpset
    make_ratexpset(const context& ctx)
    {
      return detail::make_ratexpset_registry().call(ctx->vname(false), ctx);
    }
  }
}
