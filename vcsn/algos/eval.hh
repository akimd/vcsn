#pragma once

#include <vector>

#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/misc/algorithm.hh>

namespace vcsn
{
  namespace detail
  {
    template <Automaton Aut>
    class evaluator
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "evaluate: requires free labelset");

      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using word_t = word_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;

      // state -> weight.
      using weights_t = std::vector<weight_t>;

    public:
      evaluator(const automaton_t& a)
        : aut_(a)
      {}

      weight_t operator()(const word_t& word) const
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
        v1.reserve(detail::back(aut_->all_states()) + 1);
        v1[aut_->pre()] = ws_.one();
        weights_t v2{v1};
        v2.reserve(detail::back(aut_->all_states()) + 1);

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
    private:
      automaton_t aut_;
      const weightset_t& ws_ = *aut_->weightset();
    };

  } // namespace detail

  /// General case of evaluation.
  template <Automaton Aut>
  inline
  auto
  eval(const Aut& a, const word_t_of<Aut>& w)
    -> std::enable_if_t<!context_t_of<Aut>::is_lao, weight_t_of<Aut>>
  {
    detail::evaluator<Aut> e(a);
    return e(w);
  }

  /// Evaluation for lao automaton.
  ///
  /// Require a proper automaton.
  /// In a proper lao automaton, an accepting path can only be composed by
  /// initial and final transitions. Sum the weight of these paths.
  template <Automaton Aut>
  inline
  auto
  eval(const Aut& a)
    -> std::enable_if_t<context_t_of<Aut>::is_lao, weight_t_of<Aut>>
  {
    require(is_proper(a), "eval: cannot evaluate with spontaneous transitions");
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
      eval(const automaton& aut, const label& lbl)
      {
        const auto& a = aut->as<Aut>();
        const auto& l = lbl->as<LabelSet>().label();
        auto res = ::vcsn::eval(a, l);
        const auto& ctx = a->context();
        return make_weight(*ctx.weightset(), res);
      }
    }
  }
} // namespace vcsn
