#pragma once

#include <boost/optional.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // labelset_t_of
#include <vcsn/misc/algorithm.hh> // none_of
#include <vcsn/misc/functional.hh> // less
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/type_traits.hh> // detect

namespace vcsn
{
  namespace detail
  {
    /// The type of the LabelSet::generators() member function.
    template <typename LabelSet>
    using generators_mem_fn_t = decltype(std::declval<LabelSet>().generators());

    /// Whether LabelSet features a generators() member function.
    template <typename LabelSet>
    using has_generators_mem_fn = detect<LabelSet, generators_mem_fn_t>;


    /*-----------.
    | has_one.   |
    `-----------*/

    template <typename LabelSet>
    using one_t = decltype(std::declval<LabelSet>().one());

    template <typename LabelSet>
    using has_one_mem_fn = detect<LabelSet, one_t>;


    /*-------------.
    | label_one.   |
    `-------------*/

#if defined __GNUC__ && !defined __clang__
 // GCC can't figure out that this one does return, because of the template
 // instanciation, where one of them doesn't return.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#endif

    /// Enjoy type inference.
    template <typename LabelSet>
    auto label_one(const LabelSet& ls)
      -> typename LabelSet::value_t
    {
      return static_if<LabelSet::has_one()>
        ([](const auto& ls) { return ls.one(); },
         [](const auto& ls) -> typename LabelSet::value_t
        {
          raise(ls, ": does not feature a neutral");
        })
        (ls);
    }

#if defined __GNUC__ && !defined __clang__
# pragma GCC diagnostic pop
#endif


    /*-------------------.
    | make_letterized.   |
    `-------------------*/

    /// A traits to compute the letterized context.
    ///
    /// For instance LAL -> LAL, LAW -> LAN, LAW x LAW -> LAN x LAN.
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
    /// also forbids `(a, \\e)` and `(\\e, x)` which should be kept
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
          auto end = std::mismatch(it, letters_end,
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
      for (auto l : ls.generators())
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

    template <typename LabelSet>
    typename LabelSet::letters_t
    conv_label_class_(const LabelSet& ls, std::istream& i);

    /// Read and process a class of letters.
    ///
    /// Stream \a i is after the '[', read up to the closing ']',
    /// excluded.  Apply \a fun to all the letters.  Take negation
    /// into account.  Classes can never be empty.
    //
    /// For instance "[a-d0-9_]", or "[^a-fz], or "[^]", but not
    /// "[]".
    ///
    /// Beware that symbols are processed as they are discovered, so
    /// there is no guarantee that symbols will be processed only
    /// once: `[aaa]` will process `a` three times.  If this is
    /// undesirable, use the overload below that returns a set.
    ///
    /// \pre  i does not start with ']' or '-'.
    template <typename LabelSet, typename Fun>
    void
    conv_label_class_(const LabelSet& ls, std::istream& i, Fun fun)
    {
      using letter_t = typename LabelSet::letter_t;
      using letters_t = typename LabelSet::letters_t;
      if (i.peek() == '^')
        {
          i.ignore();
          auto alphabet = letters_t{};
          for (auto l : ls.generators())
            alphabet.insert(l);
          boost::for_each(set_difference(alphabet, conv_label_class_(ls, i)),
                          fun);
        }
      else
        {
          // The last letter we read, for intervals.
          auto prev = boost::optional<letter_t>{};
          while (i.peek() != EOF && i.peek() != ']')
            if (i.peek() == '-')
              {
                i.ignore();
                // Handle a range.
                // FIXME: ls instead of ls.sname would be better.
                require(prev,
                        ls.sname(), ": letter classes cannot begin with '-'");
                require(i.peek() != ']',
                        ls.sname(), ": letter classes cannot finish with '-'");

                // [prev - l2].
                letter_t l2 = ls.get_letter(i);
                // Skip prev, which was already processed.
                auto gens = ls.generators();
                auto i = std::find(std::begin(gens), std::end(gens), *prev);
                // FIXME: Cannot use std::next here, in the case of tuples.
                if (i != std::end(gens))
                  ++i;
                for (;
                     i != std::end(gens) && *i < l2;
                     ++i)
                  fun(*i);
                // The last letter.  Do not do this in the loop,
                // we might overflow the capacity of char.
                // Check validity, so that 'z-a' is empty.
                if (*prev < l2)
                  fun(l2);

                prev = boost::none;
              }
            else
              {
                letter_t l = ls.get_letter(i);
                fun(l);
                prev = l;
              }
        }
    }

    /// Read a set of letters (hence, guaranteed in order, and unique).
    template <typename LabelSet>
    typename LabelSet::letters_t
    conv_label_class_(const LabelSet& ls, std::istream& i)
    {
      using letter_t = typename LabelSet::letter_t;
      using letters_t = typename LabelSet::letters_t;

      letters_t res;
      conv_label_class_(ls, i, [&res](letter_t l){ res.insert(l); });
      return res;
    }
  }
}
