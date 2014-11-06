#ifndef VCSN_LABELSET_LABELSET_HH
# define VCSN_LABELSET_LABELSET_HH

# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  namespace detail
  {
    /*--------------------.
    | make_nullableset.   |
    `--------------------*/

    /// The smallest nullableset which includes LabelSet.
    ///
    /// Made to be specialized (e.g., in nullableset and wordset).
    ///
    /// \tparam LabelSet  the labelset for which we look for a nullable superset
    /// \tparam Enable    place for easy SFINAE tricks (e.g., in tupleset)
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

    /*--------------------------.
    | make_word_polynomialset.  |
    `--------------------------*/

    template <typename Ctx>
    using word_polynomialset_t = polynomialset<word_context_t<Ctx>>;

    /// The polynomialset of words of a labelset (not necessarily on
    /// words itself).
    template <typename Ctx>
    inline auto
    make_word_polynomialset(const Ctx& ctx)
      -> word_polynomialset_t<Ctx>
    {
      return make_word_context(ctx);
    }
  }
}

#endif // !VCSN_LABELSET_LABELSET_HH
