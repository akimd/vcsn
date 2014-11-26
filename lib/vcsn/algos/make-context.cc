#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/signature-printer.hh>
#include <vcsn/dyn/translate.hh>
#include <vcsn/algos/make-context.hh>
#include <lib/vcsn/algos/registry.hh>


namespace vcsn
{
  namespace dyn
  {

    /*-------------.
    | context_of.  |
    `-------------*/

    REGISTER_DEFINE(context_of);
    context
    context_of(const automaton& a)
    {
      return detail::context_of_registry().call(a);
    }

    REGISTER_DEFINE(context_of_expression);
    context
    context_of(const expression& e)
    {
      return detail::context_of_expression_registry().call(e);
    }

    /*---------------.
    | make_context.  |
    `---------------*/

    REGISTER_DEFINE(make_context);
    context
    make_context(const std::string& n)
    {
      symbol sname{ast::normalize(n, false)};
      std::string full_name = ast::normalize(n, true);
      if (!detail::make_context_registry().get0({sname}))
        vcsn::dyn::compile(sname);
      return detail::make_context_registry().call({sname}, full_name);
    }


    /*-----------------.
    | make_expressionset.  |
    `-----------------*/

    REGISTER_DEFINE(make_expressionset);
    expressionset
    make_expressionset(const context& ctx, ::vcsn::rat::identities ids)
    {
      return detail::make_expressionset_registry().call(ctx, ids);
    }

    /*--------------------.
    | make_word_context.  |
    `--------------------*/

    REGISTER_DEFINE(make_word_context);
    context
    make_word_context(const context& ctx)
    {
      return detail::make_word_context_registry().call(ctx);
    }
  }
}
