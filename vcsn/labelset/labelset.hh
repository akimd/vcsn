#pragma once

#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // labelset_t_of
#include <vcsn/misc/algorithm.hh> // none_of
#include <vcsn/misc/functional.hh> // less
#include <vcsn/misc/type_traits.hh> // detect

namespace vcsn
{
  namespace detail
  {
    template <typename LabelSet>
    using genset_t = decltype(std::declval<LabelSet>().genset());

    template <typename LabelSet>
    using has_genset_mem_fn = detect<LabelSet, genset_t>;


    /*-------------.
    | label_one.   |
    `-------------*/

    /// This LabelSet's one(), if supported.
    template <typename LabelSet>
    auto label_one()
      -> enable_if_t<LabelSet::has_one(),
                     typename LabelSet::value_t>
    {
      return LabelSet::one();
    }

    template <typename LabelSet>
    ATTRIBUTE_NORETURN
    auto label_one()
      -> enable_if_t<!LabelSet::has_one(),
                     typename LabelSet::value_t>
    {
      raise(LabelSet::sname(), ": does not feature a neutral");
    }

    /// Enjoy type inference.
    template <typename LabelSet>
    auto label_one(const LabelSet&)
      -> typename LabelSet::value_t
    {
      return label_one<LabelSet>();
    }

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
    using is_letterized_t =
      bool_constant<letterized_traits<LabelSet>::is_letterized>;

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

    /*----------------------------.
    | make_nullableset_context.   |
    `----------------------------*/

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
    struct proper_traits
    {
      using type = LabelSet;
      static type value(const LabelSet& ls)
      {
        return ls;
      }
    };

    /// The type of the corresponding proper LabelSet.
    template <typename LabelSet>
    using proper_t = typename proper_traits<LabelSet>::type;

    /// The corresponding proper LabelSet.
    template <typename LabelSet>
    proper_t<LabelSet>
    make_proper(const LabelSet& ls)
    {
      return proper_traits<LabelSet>::value(ls);
    }


    /*-----------------------.
    | make_proper_context.   |
    `-----------------------*/

    template <typename Context>
    using proper_context =
      context<proper_t<labelset_t_of<Context>>,
              weightset_t_of<Context>>;

    /// From a context, its non-nullable context.
    template <typename LabelSet, typename WeightSet>
    auto
    make_proper_context(const context<LabelSet, WeightSet>& ctx)
      -> proper_context<context<LabelSet, WeightSet>>
    {
      return {make_proper(*ctx.labelset()), *ctx.weightset()};
    }


    /*---------------------.
    | make_free_context.   |
    `---------------------*/

    template <typename Context>
    using free_context =
      context<proper_t<letterized_t<labelset_t_of<Context>>>,
              weightset_t_of<Context>>;

    /// The free context for c.
    template <typename LabelSet, typename WeightSet>
    free_context<context<LabelSet, WeightSet>>
    make_free_context(const context<LabelSet, WeightSet>& c)
    {
      return {make_proper(make_letterized(*c.labelset())), *c.weightset()};
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


    /*---------------------.
    | print_label_class.   |
    `---------------------*/

    /// Print a set of labels with ranges.
    ///
    /// The order of the letters is respected; depending on the use
    /// case, you might want to call sort and unique before.
    template <typename LabelSet>
    std::ostream&
    print_label_ranges_(const LabelSet& ls,
                        const std::vector<typename LabelSet::value_t>& letters,
                        const std::vector<typename LabelSet::value_t>& alphabet,
                        std::ostream& out,
                        format fmt)
    {
      for (auto it = std::begin(letters), letters_end = std::end(letters);
           it != letters_end; ++it)
        {
          auto end
            // not std::mismatch, to please G++ 4.8.
            = vcsn::detail::mismatch(it, letters_end,
                                     boost::range::find(alphabet, *it),
                                     alphabet.end()).first;
          ls.print(*it, out, fmt);
          // No range for two letters or less.
          auto width = std::distance(it, end);
          if (2 < width)
            {
              it += width - 1;
              // Using `-` in LaTeX math mode means minus (wow, 4
              // m-words in a row), which results in a long dash, and
              // too much space around it.
              out << (fmt == format::latex ? "\\textrm{-}" : "-");
              ls.print(*it, out, fmt);
            }
        }
      return out;
    }

    /// Print a set of labels (letterized) with classes.
    ///
    /// The order of the letters is respected; depending on the use
    /// case, you might want to call sort and unique before.
    template <typename LabelSet>
    std::ostream&
    print_label_class(const LabelSet& ls,
                      const std::vector<typename LabelSet::value_t>& letters,
                      std::ostream& out,
                      format fmt)
    {
      using letters_t = std::vector<typename LabelSet::value_t>;
      // In alphabetical order.
      auto alphabet = letters_t{};
      for (auto l : ls.genset())
        alphabet.emplace_back(ls.value(l));

      out << '[';
      // If the letters are strictly increasing (hence using
      // less_equal, not just less), and are many compared to the
      // alphabet (say, at least two thirds), then we should probably
      // use negated classes instead.
      if (boost::is_sorted(letters, vcsn::less_equal<LabelSet>{})
          && 2 * boost::distance(alphabet) < 3 * boost::distance(letters))
        {
          // FIXME: we can certainly do better and avoid the
          // construction of this vector.
          auto negated = letters_t{};
          for (auto l: alphabet)
            if (none_of_equal(letters, l))
              negated.emplace_back(l);
          out << (fmt == format::latex ? "\\hat{}" : "^");
          print_label_ranges_(ls, negated, alphabet, out, fmt);
        }
      else
        print_label_ranges_(ls, letters, alphabet, out, fmt);
      out << ']';

      return out;
    }
  }
}
