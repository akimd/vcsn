#pragma once

#include <vcsn/algos/transpose.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/labelset/labelset.hh> // detail::letterized_context
#include <vcsn/labelset/word-polynomialset.hh>
#include <vcsn/misc/algorithm.hh> // front
#include <vcsn/misc/direction.hh>
#include <vcsn/weightset/polynomialset.hh>

#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/polynomial.hh>

namespace vcsn
{

  /*--------------------.
  | trie(polynomial).   |
  `--------------------*/
  namespace detail
  {
    /// Build a trie automaton (prefix-tree-like automaton).
    ///
    /// \tparam  Context  the context of the mutable_automaton to build.
    /// \tparam  Dir      if `forward` build a trie (single initial state)
    ///                   otherwise (`backward`) build a reversed trie
    ///                   (single final state, but many initial states).
    template <typename Context, direction Dir>
    class trie_builder
    {
    public:
      using context_t = Context;
      /// The type of the result.
      using automaton_t = mutable_automaton<context_t>;
      /// The type of the automaton we work on.
      using work_automaton_t
        = conditional_t<Dir == direction::forward,
                        automaton_t,
                        transpose_automaton<automaton_t>>;
      using labelset_t = labelset_t_of<context_t>;
      using letter_t = letter_t_of<context_t>;
      using word_t = word_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;
      using state_t = state_t_of<automaton_t>;
      using polynomialset_t = word_polynomialset_t<context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      trie_builder(const context_t& c)
        : res_(make_shared_ptr<work_automaton_t>(c))
      {}

      /// Add a monomial.
      /// \param l   the word to add.
      /// \param w   its associated weight.
      void add(const word_t& l, const weight_t& w)
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

      /// Get the result for the forward trie.
      template <direction D = Dir>
      auto result()
        -> enable_if_t<D == direction::forward, automaton_t>
      {
        return res_;
      }

      /// Get the result for a backward trie.
      template <direction D = Dir>
      auto result()
        -> enable_if_t<D == direction::backward, automaton_t>
      {
        return transpose(res_);
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
        auto ts = res_->out(s, l);
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
      using context_t = detail::free_context<context_t_of<PolynomialSet>>;
      auto ctx = make_free_context(ps.context());
      return {ctx};
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
        return make_automaton(trie(p.polynomialset(), p.polynomial()));
      }

      /// Bridge.
      template <typename PolynomialSet>
      automaton
      cotrie(const polynomial& poly)
      {
        const auto& p = poly->as<PolynomialSet>();
        return make_automaton(cotrie(p.polynomialset(), p.polynomial()));
      }
    }
  }

  /*--------------.
  | trie(file).   |
  `--------------*/

  /// Make a trie-like mutable_automaton for a finite series read from
  /// a stream.
  ///
  /// \param  ps  the polynomialset
  /// \param  is  the stream to read
  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  trie(const PolynomialSet& ps, std::istream& is)
  {
    auto t = detail::make_trie_builder<direction::forward>(ps);
    while (auto m = ps.conv_monomial(is))
      t.add(m.get());
    return t.result();
  }

  /// Make a trie-like mutable_automaton for a finite series read from
  /// a stream.
  ///
  /// \param  ps  the polynomialset
  /// \param  is  the stream to read
  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  cotrie(const PolynomialSet& ps, std::istream& is)
  {
    auto t = detail::make_trie_builder<direction::backward>(ps);
    while (auto m = ps.conv_monomial(is))
      t.add(m.get());
    return t.result();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (trie).
      template <typename Context, typename Istream>
      automaton
      trie_stream(const context& ctx, std::istream& is)
      {
        const auto& c = ctx->as<Context>();
        auto ps = make_word_polynomialset(c);
        return make_automaton(trie(ps, is));
      }

      /// Bridge (cotrie).
      template <typename Context, typename Istream>
      automaton
      cotrie_stream(const context& ctx, std::istream& is)
      {
        const auto& c = ctx->as<Context>();
        auto ps = make_word_polynomialset(c);
        return make_automaton(cotrie(ps, is));
      }
    }
  }
} // vcsn::
