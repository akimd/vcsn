#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/labelset/labelset.hh> // detail::letterized_context
#include <vcsn/labelset/word-polynomialset.hh>
#include <vcsn/misc/algorithm.hh> // front
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
    /// Whether \a a is standard.
    template <typename Context>
    class trie_builder
    {
    public:
      using context_t = Context;
      using automaton_t = mutable_automaton<context_t>;
      using letter_t = letter_t_of<context_t>;
      using word_t = word_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;
      using state_t = state_t_of<automaton_t>;
      using polynomialset_t = word_polynomialset_t<context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      trie_builder(const context_t& c)
        : ctx_(c)
        , res_(make_mutable_automaton(ctx_))
      {}

      /// Add a monomial.
      /// \param lbl   the word to add.
      /// \param wgt   its associated weight.
      void add(const word_t& lbl, const weight_t& wgt)
      {
        const auto& ls = *ctx_.labelset();
        state_t s = res_->pre();
        s = next_(s, ls.special());
        for (auto l: ls.letters_of(lbl))
          s = next_(s, l);
        // The final transition, where we add the weight.
        res_->add_transition(s, res_->post(), ls.special(), wgt);
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

      /// Get the result.
      automaton_t result()
      {
        return res_;
      }

    private:
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

      /// The context of the automaton: letterized.
      context_t ctx_;
      /// The automaton being built.
      automaton_t res_;
    };


    /// Instantiate a trie-builder for this type of polynomialset.
    template <typename PolynomialSet>
    trie_builder<free_context<context_t_of<PolynomialSet>>>
    make_trie_builder(const PolynomialSet& ps)
    {
      using context_t = detail::free_context<context_t_of<PolynomialSet>>;
      auto ctx = make_free_context(ps.context());
      return {ctx};
    }
  }

  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  trie(const PolynomialSet& ps, const typename PolynomialSet::value_t& p)
  {
    auto t = detail::make_trie_builder(ps);
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
    }
  }

  /*--------------.
  | trie(file).   |
  `--------------*/

  template <typename PolynomialSet>
  mutable_automaton<detail::free_context<context_t_of<PolynomialSet>>>
  trie(const PolynomialSet& ps, std::istream& is)
  {
    auto t = detail::make_trie_builder(ps);
    try
      {
        while (is.good())
          t.add(ps.conv_monomial(is));
      }
    catch (const std::runtime_error&)
      {
        // We reached EOF.
      }
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
    }
  }
} // vcsn::
