#pragma once

#include <vcsn/ctx/project-context.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/name.hh> // integral_constant

namespace vcsn
{
  /*--------------------.
  | project(context).   |
  `--------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (project).
      template <typename Context, typename Tape>
      context
      project_context(const context& ctx, integral_constant)
      {
        auto& c = ctx->as<Context>();
        return make_context(vcsn::detail::make_project_context<Tape::value>(c));
      }
    }
  }

  /*--------------------------.
  | project(expressionset).   |
  `--------------------------*/

  namespace detail
  {
    template <size_t Tape, typename Context>
    auto make_project(const expressionset<Context>& rs)
      -> expressionset<decltype(make_project_context<Tape>(rs.context()))>
    {
      return {make_project_context<Tape>(rs.context()),
              rs.identities()};
    }
  }
}
