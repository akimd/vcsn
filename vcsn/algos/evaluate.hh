#pragma once

#include <vector>
#include <queue>

#include <vcsn/algos/is-free.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/core/automaton.hh> // out
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/word-polynomialset.hh>
#include <vcsn/misc/algorithm.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{
  namespace detail
  {
    /// Evaluate a word on an automaton.
    template <Automaton Aut>
    class evaluator
    {
      using automaton_t = Aut;
      using labelset_t = labelset_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using word_t = word_t_of<automaton_t>;
      using context_t = context_t_of<automaton_t>;
      using wps_t = word_polynomialset_t<context_t>;
      using polynomial_t = typename wps_t::value_t;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;

      /// state -> weight.
      using weights_t = std::vector<weight_t>;

    public:
      evaluator(const automaton_t& a)
        : aut_(a)
      {}

      struct labeled_weight
      {
        weight_t w;
        word_t l;
        state_t s;
        labeled_weight(weight_t w_, word_t l_, state_t s_)
        {
          w = w_;
          l = l_;
          s = s_;
        }
      };

      /// Evaluation of a word.
      weight_t operator()(const word_t& word) const
      {
        if (is_free(aut_))
          return eval_free_(word);
        else
          return eval_non_free_(word);
      }

      /// Evaluation of a polynomial.
      weight_t operator()(const polynomial_t& poly) const
      {
        // Since the algorithm itself is about circulating monomials,
        // it's natural to load the queue with a polynonial, rather
        // than performing a full evaluation for each monomial of the
        // polynonial.  But this benchmark:
        //
        //     import vcsn
        //     c = vcsn.context('law(a-z), z')
        //     a = c.de_bruijn(100)
        //     p = c.polynomial('\z')
        //     for i in range(1, 100):
        //         p = p + c.polynomial('abcxyz' * i)
        //     print(vcsn.timeit(lambda: a(p)))
        //
        // shows no improvement if we do so.
        auto res = ws_.zero();
        for (const auto& m: poly)
          res = ws_.add(res, ws_.mul(weight_of(m), operator()(label_of(m))));
        return res;
      }

    private:
      /// General case of evaluation: load the word on `pre`, and let
      /// it follow the transitions, gaining weight (ah ah), and
      /// loosing letters.
      weight_t eval_non_free_(const word_t& word) const
      {
        const auto wordset = make_wordset(*aut_->labelset());
        auto res = ws_.zero();

        // A queue of current word states in the automaton.
        auto q = std::queue<labeled_weight>{};
        q.emplace(ws_.one(), wordset.delimit(word), aut_->pre());

        while (!q.empty())
          {
            auto cur = q.front();
            q.pop();

            for (auto const t : all_out(aut_, cur.s))
              if (aut_->dst_of(t) == aut_->post() && wordset.is_special(cur.l))
                // The word is accepted.
                {
                  cur.w = ws_.mul(cur.w, aut_->weight_of(t));
                  res = ws_.add(res, cur.w);
                }
              else if (auto new_word
                        = wordset.maybe_ldivide(ls_.word(aut_->label_of(t)), cur.l))
                q.emplace(
                    ws_.mul(cur.w, aut_->weight_of(t)),
                    std::move(*new_word),
                    aut_->dst_of(t));
          }

        return res;
      }

      // This does not work with wordset, thus the is_letterized requirement.
      template <typename LabelSet = labelset_t>
      std::enable_if_t<LabelSet::is_letterized(),
                       weight_t>
      eval_free_(const word_t& word) const
      {
        // An array indexed by state numbers.
        //
        // Do not use braces (v1{size, ws_.zero()}): the type of zero
        // might result in the compiler believing we are building a
        // vector with two values: size and zero.
        //
        // We start with just two states numbered 0 and 1: pre() and
        // post().
        auto v1 = weights_t(2, ws_.zero());
        v1.reserve(states_size(aut_));
        v1[aut_->pre()] = ws_.one();
        auto v2 = v1;
        v2.reserve(states_size(aut_));

        // Computation.
        for (const auto l : ls_.letters_of(ls_.delimit(word)))
          {
            v2.assign(v2.size(), ws_.zero());
            for (size_t s = 0; s < v1.size(); ++s)
              if (!ws_.is_zero(v1[s])) // delete if bench >
                for (const auto t : out(aut_, s, l))
                  {
                    const auto dst = aut_->dst_of(t);
                    // Make sure the vectors are large enough for dst.
                    // Exponential growth on the capacity, but keep
                    // the actual size as small as possible.
                    if (v2.size() <= dst)
                      {
                        auto capacity = v2.capacity();
                        while (capacity <= dst)
                          capacity *= 2;
                        v1.reserve(capacity);
                        v2.reserve(capacity);
                        v1.resize(dst + 1, ws_.zero());
                        v2.resize(dst + 1, ws_.zero());
                      }
                    // Introducing a reference to v2[dst] is tempting,
                    // but won't work for std::vector<bool>.  FIXME:
                    // Specialize for Boolean?  Or introduce add_here.
                    v2[dst] =
                      ws_.add(v2[dst],
                              ws_.mul(v1[s], aut_->weight_of(t)));
                  }
            std::swap(v1, v2);
          }
        return v1[aut_->post()];
      }

      // We should never call this with a non-letterized labelset:
      // only lal or tupleset of lal can be free.
      template <typename LabelSet = labelset_t>
      std::enable_if_t<!LabelSet::is_letterized(),
                       weight_t>
      eval_free_(const word_t&) const
      {
        BUILTIN_UNREACHABLE();
      }

      automaton_t aut_;
      const weightset_t& ws_ = *aut_->weightset();
      const labelset_t& ls_ = *aut_->labelset();
    };

  } // namespace detail

  /// General case of evaluation.
  template <Automaton Aut>
  auto
  evaluate(const Aut& a, const word_t_of<Aut>& w)
    -> std::enable_if_t<!context_t_of<Aut>::is_lao, weight_t_of<Aut>>
  {
    auto e = detail::evaluator<Aut>{a};
    return e(w);
  }

  /// Evaluation for lao automaton.
  ///
  /// Requires a proper automaton.
  /// In a proper lao automaton, an accepting path can only be composed by
  /// initial and final transitions. Sum the weight of these paths.
  template <Automaton Aut>
  auto
  evaluate(const Aut& a, const label_t_of<Aut>& = {})
    -> std::enable_if_t<context_t_of<Aut>::is_lao, weight_t_of<Aut>>
  {
    require(is_proper(a),
            "evaluate: cannot evaluate with spontaneous transitions");
    const auto& ws = *a->weightset();
    auto res = ws.zero();
    for (const auto init_tr: initial_transitions(a))
      {
        const auto s = a->dst_of(init_tr);
        const auto w = a->weight_of(init_tr);
        for (const auto out: all_out(a, s))
          {
            assert(a->dst_of(out) == a->post());
            res = ws.add(res, ws.mul(w, a->weight_of(out)));
          }
      }
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename LabelSet>
      weight
      evaluate(const automaton& aut, const label& lbl)
      {
        using ctx_t = context_t_of<Aut>;
        // This is utterly wrong, as we do not support
        // lat<expressionset, ...>.  but currently we have no means to
        // tell the difference.  We probably need something like
        // "is_graduated".
        constexpr auto valid
          = (ctx_t::is_lal || ctx_t::is_lao || ctx_t::is_lat || ctx_t::is_law);
        const auto& a= aut->as<Aut>();
        if constexpr (valid)
          {
            auto res = ::vcsn::evaluate(a, lbl->as<LabelSet>().value());
            return {*a->weightset(), res};
          }
        else
            raise("evaluate: unsupported labelset: ", *a->labelset());
      }
    }
  }

  /// Evaluation of a polynomial.
  template <Automaton Aut>
  auto
  evaluate(const Aut& a,
           const typename detail::word_polynomialset_t<context_t_of<Aut>>::value_t& p)
    -> weight_t_of<Aut>
  {
    auto e = detail::evaluator<Aut>{a};
    return e(p);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (evaluate).
      template <Automaton Aut, typename PolynomialSet>
      weight
      evaluate_polynomial(const automaton& aut, const polynomial& poly)
      {
        const auto& a = aut->as<Aut>();
        const auto& p = poly->as<PolynomialSet>().value();
        auto res = ::vcsn::evaluate(a, p);
        return {*a->weightset(), res};
      }
    }
  }
} // namespace vcsn
