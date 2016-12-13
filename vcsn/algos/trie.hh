#pragma once

#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton.hh> // out
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/labelset/labelset.hh> // detail::letterized_context
#include <vcsn/labelset/word-polynomialset.hh>
#include <vcsn/misc/algorithm.hh> // front
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/regex.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  /*-----------------.
  | Function tags.   |
  `-----------------*/

  CREATE_FUNCTION_TAG(trie);
  CREATE_FUNCTION_TAG(cotrie);

  template <>
  struct function_prop<trie_ftag>
  {
    static const bool invalidate = false;

    static auto& ignored_prop()
    {
      static auto ignored = ignored_prop_t{};
      return ignored;
    }

    static auto& updated_prop()
    {
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      static auto updated_prop = create_updated_prop(
        {
          // By construction, the trie is deterministic (see notebook).
          { is_deterministic_ptag::id(), boost::any{true} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };


  namespace detail
  {
    /// Turn a label into a parsable label: escape special characters.
    std::string quote(const std::string& s)
    {
      if (s == "\\e"
          || (s.size() == 1 && std::isalnum(s[0])))
        return s;
      else
        {
          // Backslash backslashes and quotes.
          static auto re = std::regex{"['\\\\ \\[\\]()+,|<>]"};
          return std::regex_replace(s, re, "\\$&");
        }
    }
}

  /*--------------------.
  | trie(polynomial).   |
  `--------------------*/
  namespace detail
  {
    /// Build a trie automaton (prefix-tree-like automaton).
    ///
    /// \tparam  Context  the context of the mutable_automaton to build.
    ///                   It is typically a letterset, even though we feed
    ///                   it with words.
    /// \tparam  Dir      if `forward` build a trie (single initial state)
    ///                   otherwise (`backward`) build a reversed trie
    ///                   (single final state, but many initial states).
    template <typename Context, direction Dir>
    class trie_builder
    {
    public:
      using context_t = Context;
      using self_t = trie_builder;
      /// The type of the result.
      using automaton_t = mutable_automaton<context_t>;
      /// The type of the automaton we work on.
      using work_automaton_t
        = std::conditional_t<Dir == direction::forward,
                        automaton_t,
                        transpose_automaton<automaton_t>>;
      /// The input labelset, free/letterized or not.
      using labelset_t = labelset_t_of<context_t>;
      using letter_t = letter_t_of<context_t>;
      using word_t = word_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;
      using state_t = state_t_of<automaton_t>;

      /// Polynomialset for the input: weighted words.
      using polynomialset_t = word_polynomialset_t<context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      trie_builder(const context_t& c)
        : res_(make_shared_ptr<work_automaton_t>(c))
      {}

      /// Add a monomial.
      /// \param l   the word to add.
      /// \param w   its associated weight.
      void add(const word_t& l, const weight_t& w = weightset_t::one())
      {
        add_(l, w);
      }

      /// Add a monomial.
      void add(const monomial_t& m)
      {
        add(label_of(m), weight_of(m));
      }

      /// Add a polynomial.
      void add(const polynomial_t& p)
      {
        for (const auto& m: p)
          add(m);
      }

      /// Add all the words (one per line) in this stream.
      void add_words(std::istream& is)
      {
        auto ls = make_wordset(*ctx_.labelset());
        ls.open(true);
        std::string buf;
        while (getline(is, buf))
          {
            auto w = conv(ls, quote(buf));
            add(w);
          }
      }

      /// Add all the monomials (one per line) in this stream.
      void add_monomials(std::istream& is)
      {
        auto ps = make_word_polynomialset(ctx_);
        while (auto m = ps.conv_monomial(is))
          add(*m);
      }

      /// Add all the monomials in this stream.
      void add(std::istream& is, const std::string& format)
      {
        static const auto map = getarg<void(self_t::*)(std::istream&)>
          {
            "trie format",
            {
              {"default",   "monomials"},
              {"monomials", &self_t::add_monomials},
              {"words",     &self_t::add_words},
            }
          };
        (this->*(map[format]))(is);
      }

      /// Get the result for the forward trie.
      template <direction D = Dir>
      auto result()
        -> std::enable_if_t<D == direction::forward, automaton_t>
      {
        res_->properties().update(trie_ftag{});
        return res_;
      }

      /// Get the result for a backward trie.
      template <direction D = Dir>
      auto result()
        -> std::enable_if_t<D == direction::backward, automaton_t>
      {
        auto res = transpose(res_);
        res->properties().update(cotrie_ftag{});
        return res;
      }

    private:
      /// Add a monomial.  Take the direction into account (i.e.,
      /// transpose the word if needed).
      ///
      /// \param lbl   the word to add.
      /// \param wgt   its associated weight.
      void add_(const word_t& lbl, const weight_t& wgt)
      {
        const auto& ls = *ctx_.labelset();
        state_t s = res_->pre();
        s = next_(s, ls.special());
        for (auto l: ls.letters_of_padded(Dir == direction::forward
                                          ? lbl
                                          : ls.transpose(lbl),
                                          padding_))
          s = next_(s, l);
        // The final transition, where we add the weight.
        res_->add_transition(s, res_->post(), ls.special(), wgt);
      }

      /// Follow a transition, possibly creating it.
      /// \returns  the destination state.
      state_t next_(state_t s, letter_t l)
      {
        auto ts = out(res_, s, l);
        assert(ts.size() == 0 || ts.size() == 1);
        if (ts.empty())
          {
            auto d = res_->new_state();
            res_->new_transition(s, d, l);
            return d;
          }
        else
          return res_->dst_of(front(ts));
      }

      /// The automaton being built.
      work_automaton_t res_;
      /// The context of the automaton: letterized.
      const context_t& ctx_ = res_->context();
      /// Padding, in case it is needed.
      typename letterized_t<labelset_t>::value_t padding_
        = letterized_t<labelset_t>::one();
    };

    /// Instantiate a trie-builder for this type of polynomialset.
    template <direction Dir, typename PolynomialSet>
    trie_builder<free_context<context_t_of<PolynomialSet>>, Dir>
    make_trie_builder(const PolynomialSet& ps)
    {
      return {make_free_context(ps.context())};
    }
  }

  /// Make a trie-like mutable_automaton for a finite series given as
  /// a polynomial.
  ///
  /// \param  ps  the polynomialset
  /// \param  p   the polynomial that specifies the series.
  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  trie(const PolynomialSet& ps, const typename PolynomialSet::value_t& p)
  {
    auto t = detail::make_trie_builder<direction::forward>(ps);
    t.add(p);
    return t.result();
  }

  /// Make a cotrie-like mutable_automaton for a finite series given as
  /// a polynomial.
  ///
  /// \param  ps  the polynomialset
  /// \param  p   the polynomial that specifies the series.
  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  cotrie(const PolynomialSet& ps, const typename PolynomialSet::value_t& p)
  {
    auto t = detail::make_trie_builder<direction::backward>(ps);
    t.add(p);
    return t.result();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSet>
      automaton
      trie(const polynomial& poly)
      {
        const auto& p = poly->as<PolynomialSet>();
        return trie(p.valueset(), p.value());
      }

      /// Bridge.
      template <typename PolynomialSet>
      automaton
      cotrie(const polynomial& poly)
      {
        const auto& p = poly->as<PolynomialSet>();
        return cotrie(p.valueset(), p.value());
      }
    }
  }

  /*--------------.
  | trie(file).   |
  `--------------*/

  /// Make a trie-like mutable_automaton for a finite series read from
  /// a stream.
  ///
  /// \param ps      the polynomialset
  /// \param is      the stream to read
  /// \param format  the format of the file: "words" or "monomials"
  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  trie(const PolynomialSet& ps, std::istream& is,
       const std::string& format = "default")
  {
    auto t = detail::make_trie_builder<direction::forward>(ps);
    t.add(is, format);
    return t.result();
  }

  /// Make a trie-like mutable_automaton for a finite series read from
  /// a stream.
  ///
  /// \param ps      the polynomialset
  /// \param is      the stream to read
  /// \param format  the format of the file: "words" or "monomials"
  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  cotrie(const PolynomialSet& ps, std::istream& is,
         const std::string& format = "default")
  {
    auto t = detail::make_trie_builder<direction::backward>(ps);
    t.add(is, format);
    return t.result();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (trie).
      template <typename Context, typename Istream, typename String>
      automaton
      trie_stream(const context& ctx, std::istream& is,
                  const std::string& format)
      {
        const auto& c = ctx->as<Context>();
        auto ps = make_word_polynomialset(c);
        return trie(ps, is, format);
      }

      /// Bridge (cotrie).
      template <typename Context, typename Istream, typename String>
      automaton
      cotrie_stream(const context& ctx, std::istream& is,
                    const std::string& format)
      {
        const auto& c = ctx->as<Context>();
        auto ps = make_word_polynomialset(c);
        return cotrie(ps, is, format);
      }
    }
  }
} // vcsn::
