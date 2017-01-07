#pragma once

#include <vector>
#include <queue>

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
      using wordset_t = law_t<labelset_t>;
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

      template <typename LabelSet = labelset_t>
      std::enable_if_t<!LabelSet::is_free(),
                      weight_t>
      operator()(const word_t& word) const
      {
        // Initialization.
        weight_t res = ws_.zero();

        // A queue of current word states in the automaton.
        auto q = std::queue<labeled_weight>{};
        q.emplace(ws_.one(), wordset_.delimit(word), aut_->pre());

        while (!q.empty())
          {
            auto cur = q.front();
            q.pop();

            for (auto t : all_out(aut_, cur.s))
              if (aut_->dst_of(t) == aut_->post() && wordset_.is_special(cur.l))
                // The word is accepted.
                {
                  cur.w = ws_.mul(cur.w, aut_->weight_of(t));
                  res = ws_.add(res, cur.w);
                }
              else if (auto new_word
                        = wordset_.maybe_ldivide(ls_.word(aut_->label_of(t)), cur.l))
                q.emplace(
                    ws_.mul(cur.w, aut_->weight_of(t)),
                    std::move(*new_word),
                    aut_->dst_of(t));
          }

        return res;
      }

      template <typename LabelSet = labelset_t>
      std::enable_if_t<LabelSet::is_free(),
                      weight_t>
      operator()(const word_t& word) const
      {
        // Initialization.
        const weight_t zero = ws_.zero();

        // An array indexed by state numbers.
        //
        // Do not use braces (v1{size, zero}): the type of zero might
        // result in the compiler believing we are building a vector
        // with two values: size and zero.
        //
        // We start with just two states numbered 0 and 1: pre() and
        // post().
        weights_t v1(2, zero);
        v1.reserve(states_size(aut_));
        v1[aut_->pre()] = ws_.one();
        weights_t v2{v1};
        v2.reserve(states_size(aut_));

        // Computation.
        auto ls = *aut_->labelset();
        for (auto l : ls.letters_of(ls.delimit(word)))
          {
            v2.assign(v2.size(), zero);
            for (size_t s = 0; s < v1.size(); ++s)
              if (!ws_.is_zero(v1[s])) // delete if bench >
                for (auto t : out(aut_, s, l))
                  {
                    // Make sure the vectors are large enough for dst.
                    // Exponential growth on the capacity, but keep
                    // the actual size as small as possible.
                    auto dst = aut_->dst_of(t);
                    if (v2.size() <= dst)
                      {
                        auto capacity = v2.capacity();
                        while (capacity <= dst)
                          capacity *= 2;
                        v1.reserve(capacity);
                        v2.reserve(capacity);
                        v1.resize(dst + 1, zero);
                        v2.resize(dst + 1, zero);
                      }
                    // Introducing a reference to v2[aut_->dst_of(tr)] is
                    // tempting, but won't work for std::vector<bool>.
                    // FIXME: Specialize for Boolean?
                    v2[dst] =
                      ws_.add(v2[dst],
                              ws_.mul(v1[s], aut_->weight_of(t)));
                  }
            std::swap(v1, v2);
          }
        return v1[aut_->post()];
      }

      /// Polynomial implementation.
      weight_t operator()(const polynomial_t& poly) const
      {
        weight_t res = ws_.zero();
        for (const auto& m: poly)
          res = ws_.add(res, ws_.mul(weight_of(m), (*this)(label_of(m))));

        return res;
      }

    private:
      automaton_t aut_;
      const weightset_t& ws_ = *aut_->weightset();
      const labelset_t& ls_ = *aut_->labelset();
      const wordset_t wordset_ = make_wordset(*aut_->labelset());
      const wps_t wps_ = make_word_polynomialset(aut_->context());
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
  /// Require a proper automaton.
  /// In a proper lao automaton, an accepting path can only be composed by
  /// initial and final transitions. Sum the weight of these paths.
  template <Automaton Aut>
  auto
  evaluate(const Aut& a, const label_t_of<Aut>& = {})
    -> std::enable_if_t<context_t_of<Aut>::is_lao, weight_t_of<Aut>>
  {
    require(is_proper(a), "evaluate: cannot evaluate with spontaneous transitions");
    const auto& ws = *a->weightset();
    auto res = ws.zero();
    for (auto init_tr: initial_transitions(a))
      {
        auto s = a->dst_of(init_tr);
        auto w = a->weight_of(init_tr);
        for (auto out: all_out(a, s))
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
          = (ctx_t::is_lal || ctx_t::is_lan || ctx_t::is_lao
             || ctx_t::is_lat || ctx_t::is_law);
        return vcsn::detail::static_if<valid>
          ([](const auto& a, const auto& l) -> weight
           {
             auto res = ::vcsn::evaluate(a, l);
             return {*a->weightset(), res};
           },
           [](const auto& a, const auto&) -> weight
           {
             raise("evaluate: unsupported labelset: ",
                   *a->labelset());
           })
          (aut->as<Aut>(), lbl->as<LabelSet>().value());
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
