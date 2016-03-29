#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/labelset/tupleset.hh>

namespace vcsn
{

  /*--------------------.
  | project(context).   |
  `--------------------*/

  namespace detail
  {
    /// The resulting apparent context when keeping only tape Tape.
    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto project(const context<LabelSet, WeightSet>& ctx)
      -> std::enable_if_t<is_multitape<LabelSet>{},
                          project_context<Tape, context<LabelSet, WeightSet>>>
    {
      // FIXME: The following assert works only for tupleset. Handle
      // the case of expressionset.
      //
      //      static_assert(Tape < LabelSet::size(),
      //                    "project: tape index out of bounds");
      return {ctx.labelset()->template project<Tape>(), *ctx.weightset()};
    }

    /// When the labelset is not a tupleset, require the tape to be 0,
    /// and return it.
    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto project(const context<LabelSet, WeightSet>& ctx)
      -> std::enable_if_t<!is_multitape<LabelSet>{},
                          context<LabelSet, WeightSet>>
    {
      static_assert(Tape == 0,
                    "project: cannot extract non-0 tape from a non tupleset"
                    " labelset");
      return ctx;
    }
  }
}
