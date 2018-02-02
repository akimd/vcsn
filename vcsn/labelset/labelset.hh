#pragma once

#include <optional>

#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/is_sorted.hpp>

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // labelset_t_of
#include <vcsn/misc/algorithm.hh> // none_of
#include <vcsn/misc/functional.hh> // less
#include <vcsn/misc/random.hh>
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/type_traits.hh> // detect

namespace vcsn
{
  namespace detail
  {
    /// The type of the LabelSet::generators() member function.
    template <typename LabelSet>
    using generators_mem_fn_t
      = decltype(std::declval<LabelSet>().generators());

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


    /*-------------------.
    | make_letterized.   |
    `-------------------*/

    /// A traits to compute the letterized context.
    ///
    /// For instance LAL -> LAL, LAW -> LAL, LAW x LAW -> LAL x LAL.
    template <typename LabelSet>
    struct letterized_traits
    {
      static constexpr bool is_letterized = true;

      using labelset_t = LabelSet;
      static labelset_t labelset(const labelset_t& ls)
      {
        return {ls.genset()};
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


    /*---------------------.
    | make_free_context.   |
    `---------------------*/

    template <typename Context>
    using free_context =
      context<letterized_t<labelset_t_of<Context>>,
              weightset_t_of<Context>>;

    /// The free context for c.
    template <typename LabelSet, typename WeightSet>
    free_context<context<LabelSet, WeightSet>>
    make_free_context(const context<LabelSet, WeightSet>& c)
    {
      return {make_letterized(*c.labelset()), *c.weightset()};
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
                                   boost::find(alphabet, *it),
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
          auto prev = std::optional<letter_t>{};
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
                // boost::find is ambiguous between Boost.Range and
                // Boost.Algorithms.
                auto i = boost::range::find(gens, *prev);
                // FIXME: Cannot use std::next here, in the case of tuples.
                if (i != std::end(gens))
                  ++i;
                for (;
                     i != std::end(gens) && ls.less(*i, l2);
                     ++i)
                  fun(*i);

                // The last letter.  Do not do this in the loop, we
                // might overflow the capacity of char.  Check
                // validity, so that 'z-a' is empty.
                if (ls.less(*prev, l2))
                  fun(l2);

                prev = std::nullopt;
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

      auto res = letters_t{};
      conv_label_class_(ls, i, [&res](letter_t l){ res.insert(l); });
      return res;
    }
  }
}
