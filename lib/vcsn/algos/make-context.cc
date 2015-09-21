#include <lib/vcsn/algos/registry.hh>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/signature-printer.hh>
#include <vcsn/dyn/translate.hh>


namespace vcsn
{
  namespace dyn
  {

    /*---------------.
    | make_context.  |
    `---------------*/

    REGISTER_DEFINE(make_context);
    context
    make_context(const std::string& n)
    {
      symbol sname{ast::normalize_context(n, false)};
      std::string full_name = ast::normalize_context(n, true);
      if (!detail::make_context_registry().get0({sname}))
        vcsn::dyn::compile(sname);
      return detail::make_context_registry().call({sname}, full_name);
    }
  }
}
