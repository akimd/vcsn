#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/fwd.hh>
#include <vcsn/core/mutable-automaton.hh>
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
      -> enable_if_t<context<LabelSet, WeightSet>::is_lat,
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
      -> enable_if_t<!context<LabelSet, WeightSet>::is_lat,
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


  /*----------------------.
  | project(automaton).   |
  `----------------------*/

  template <std::size_t Tape, typename Aut>
  auto project(const Aut& aut)
    -> mutable_automaton<detail::project_context<Tape, context_t_of<Aut>>>
  {
    static_assert(Tape < labelset_t_of<Aut>::size(),
                  "project: invalid tape number");
    auto res =
      make_mutable_automaton(detail::make_project_context<Tape>(aut->context()));
    copy_into(focus<Tape>(aut), res, false);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Tape>
      automaton
      project(const automaton& aut, integral_constant)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(vcsn::project<Tape::value>(a));
      }
    }
  }
}

// Yet, this is ugly, no doubt about it.  But it will resolved
// eventually, when `project` is implemented independently of focus.
// But currently, focus.hh needs project.hh's definition of
// project_context, and project.hh's `project(automaton)` needs
// focus.hh's `focus(automaton)`.
#include <vcsn/algos/focus.hh>
