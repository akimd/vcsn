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
      std::string name = ast::normalize(n);
      // If the context is not known, try to compile and load it
      // first.
      auto sname = detail::context_base::sname(name);
      // Turn the context name into a signature: {sname}.
      if (!detail::make_context_registry().get0({sname}))
        vcsn::dyn::compile(sname);
      return detail::make_context_registry().call({sname}, name);
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
