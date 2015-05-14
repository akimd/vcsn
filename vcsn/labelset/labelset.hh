#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // labelset_t_of

namespace vcsn
{
  namespace detail
  {

    /*-------------------.
    | make_letterized.   |
    `-------------------*/

    template <typename LabelSet>
    struct letterized_traits
    {
      static constexpr bool is_letterized = true;

      using labelset_t = LabelSet;
      static labelset_t labelset(const labelset_t& ls)
      {
        return std::make_shared<labelset_t>(labelset_t{ls.genset()});
      }
    };

    template <typename LabelSet>
    using letterized_t =
      typename letterized_traits<LabelSet>::labelset_t;

    template <typename LabelSet>
    letterized_t<LabelSet>
    make_letterized(const LabelSet& ls)
    {
      return letterized_traits<LabelSet>::labelset(ls);
    }


    /*---------------------------.
    | make_letterized_context.   |
    `---------------------------*/

    template <typename Context>
    using letterized_context =
      context<letterized_t<labelset_t_of<Context>>,
              weightset_t_of<Context>>;

    /// The letterized context for c.
    template <typename LabelSet, typename WeightSet>
    letterized_context<context<LabelSet, WeightSet>>
    make_letterized_context(const context<LabelSet, WeightSet>& c)
    {
      return {make_letterized(*c.labelset()), *c.weightset()};
    }


    /*--------------------.
    | make_nullableset.   |
    `--------------------*/

    /// The smallest nullableset which includes LabelSet.
    ///
    /// Made to be specialized (e.g., in nullableset and wordset).
    ///
    /// \tparam LabelSet
    ///         the labelset for which we look for a nullable superset.
    /// \tparam Enable
    ///         place for easy SFINAE tricks (e.g., in tupleset).
    template <typename LabelSet,
              typename Enable = void>
    struct nullableset_traits
    {};

    /// The smallest nullableset that includes LabelSet.
    template <typename LabelSet>
    using nullableset_t = typename nullableset_traits<LabelSet>::type;

    /// The nullableset of a labelset.
    template <typename LabelSet>
    inline nullableset_t<LabelSet>
    make_nullableset(const LabelSet& ls)
    {
      return nullableset_traits<LabelSet>::value(ls);
    };

    /*-------------------------.
    | make_nullable_context.   |
    `-------------------------*/

    template <typename Ctx>
    using nullableset_context_t
      = context<nullableset_t<labelset_t_of<Ctx>>, weightset_t_of<Ctx>>;

    /// The nullableset context of a context.
    template <typename LabelSet, typename WeightSet>
    inline nullableset_context_t<context<LabelSet, WeightSet>>
    make_nullableset_context(const context<LabelSet, WeightSet>& ctx)
    {
      return {make_nullableset(*ctx.labelset()), *ctx.weightset()};
    }


    /*---------------.
    | make_proper.   |
    `---------------*/

    /// From a labelset, its non-nullable labelset.
    ///
    /// Unfortunately cannot be always done.  For instance,
    /// `tupleset<nullableset<letterset>, nullableset<letterset>>`
    /// cannot be turned in `tupleset<letterset, letterset>`, as it
    /// also forbids `(a, \e)` and `(\e, x)` which should be kept
    /// legitimate.
    template <typename LabelSet>
    struct proper_labelset
    {
      using type = LabelSet;
      static type value(const LabelSet& ls)
      {
        return ls;
      }
    };

    /// From a context, its non-nullable context.
    template <typename LabelSet, typename WeightSet>
    auto
    proper_context(const context<LabelSet, WeightSet>& ctx)
      -> context<typename proper_labelset<LabelSet>::type, WeightSet>
    {
      using proper_labelset = proper_labelset<LabelSet>;
      const typename proper_labelset::type& ls
        = proper_labelset::value(*ctx.labelset());
      const WeightSet& ws = *ctx.weightset();
      return {ls, ws};
    }


    /*---------------.
    | make_wordset.  |
    `---------------*/

    /// The LAW from a LAL.
    ///
    /// Made to be specialized (e.g., in wordset).
    template <typename ValueSet>
    struct law_traits
    {};

    /// The smallest wordset that includes LabelSet.
    template <typename LabelSet>
    using law_t = typename law_traits<LabelSet>::type;

    /// The wordset of a labelset.
    template <typename LabelSet>
    inline law_t<LabelSet>
    make_wordset(const LabelSet& ls)
    {
      return law_traits<LabelSet>::value(ls);
    };

    /*--------------------.
    | make_word_context.  |
    `--------------------*/

    template <typename Ctx>
    using word_context_t
      = context<law_t<labelset_t_of<Ctx>>, weightset_t_of<Ctx>>;

    /// The wordset context of a context.
    template <typename LabelSet, typename WeightSet>
    inline word_context_t<context<LabelSet, WeightSet>>
    make_word_context(const context<LabelSet, WeightSet>& ctx)
    {
      return {make_wordset(*ctx.labelset()), *ctx.weightset()};
    }
  }
}
