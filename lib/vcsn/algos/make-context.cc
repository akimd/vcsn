#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/translate.hh>
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
      // If the context is not known, try to compile and load it
      // first.
      auto sname = detail::context_base::sname(name);
      // Turn the context name into a signature: {sname}.
      if (!detail::make_context_registry().get0({sname}))
        {
          vcsn::dyn::detail::translation translate;
          translate.compile(sname);
        }
      return detail::make_context_registry().call({sname}, name);
    }


    /*-----------------.
    | make_ratexpset.  |
    `-----------------*/

    REGISTER_DEFINE(make_ratexpset);

    ratexpset
    make_ratexpset(const context& ctx)
    {
      return detail::make_ratexpset_registry().call(ctx);
    }
  }
}
