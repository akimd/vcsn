#pragma once

#include <vcsn/dyn/context.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/name.hh> // integral_constant

namespace vcsn
{

  /*-----------------.
  | project_context. |
  `-----------------*/

  namespace detail
  {
    /// The resulting apparent context when keeping only tape Tape.
    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto make_project_context(const context<LabelSet, WeightSet>& ctx)
      -> std::enable_if_t<context<LabelSet, WeightSet>::is_lat,
                     project_context<Tape, context<LabelSet, WeightSet>>>
    {
      static_assert(Tape < LabelSet::size(),
                    "project: tape index out of bounds");
      return {ctx.labelset()->template set<Tape>(), *ctx.weightset()};
    }

    /// When the labelset is not a tupleset, require the tape to be 0,
    /// and return it.
    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto make_project_context(const context<LabelSet, WeightSet>& ctx)
      -> std::enable_if_t<!context<LabelSet, WeightSet>::is_lat,
                     context<LabelSet, WeightSet>>
    {
      static_assert(Tape == 0,
                    "project: cannot extract non-0 tape from a non tupleset"
                    " labelset");
      return ctx;
    }
  }


  /*------------------------.
  | project(expressionset). |
  `------------------------*/

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
}
