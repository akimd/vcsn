#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/labelset/labelset.hh> // detail::letterized_context
#include <vcsn/labelset/word-polynomialset.hh>
#include <vcsn/misc/algorithm.hh> // front
#include <vcsn/weightset/polynomialset.hh>

// FIXME: we should not need these two.
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/nullableset.hh>

#include <vcsn/dyn/automaton.hh>
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
  }

  template <typename PolynomialSet>
  mutable_automaton<detail::letterized_context<context_t_of<PolynomialSet>>>
  trie(const PolynomialSet& ps, const typename PolynomialSet::value_t& p)
  {
    using context_t = detail::letterized_context<context_t_of<PolynomialSet>>;
    auto ctx = letterize_context(ps.context());
    auto t = detail::trie_builder<context_t>{ctx};
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
} // vcsn::
