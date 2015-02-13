#pragma once

#include <vector>

#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/misc/algorithm.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Aut>
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
        : a_(a)
        , ws_(*a_->weightset())
      {}

      weight_t operator()(const word_t& word) const
      {
        // Initialization.
        const weight_t zero = ws_.zero();
        // FIXME: a perfect job for a sparse array: most of the states
        // will be not visited, nevertheless, because we iterate on
        // all the states, they are costly at each iteration.

        /// An array indexed by state numbers.
        size_t last_state = detail::back(a_->all_states());
        // Do not use braces (v1{size, zero}): the type of zero might
        // result in the compiler believing we are building a vector
        // with two values: a_->num_all_states() and zero.
        weights_t v1(last_state + 1, zero);
        v1[a_->pre()] = ws_.one();
        weights_t v2{v1};

        // Computation.
        auto ls = *a_->labelset();
        for (auto l : ls.letters_of(ls.delimit(word)))
          {
            v2.assign(v2.size(), zero);
            for (size_t s = 0; s < v1.size(); ++s)
              if (!ws_.is_zero(v1[s])) // delete if bench >
                for (auto t : a_->out(s, l))
                  // Introducing a reference to v2[a_->dst_of(tr)] is
                  // tempting, but won't work for std::vector<bool>.
                  // FIXME: Specialize for Boolean?
                  v2[a_->dst_of(t)] =
                    ws_.add(v2[a_->dst_of(t)],
                            ws_.mul(v1[s], a_->weight_of(t)));
            std::swap(v1, v2);
          }
        return v1[a_->post()];
      }
    private:
      const automaton_t& a_;
      const weightset_t& ws_;
    };

  } // namespace detail

  template <typename Aut>
  inline
  auto
  eval(const Aut& a, const word_t_of<Aut>& w)
    -> weight_t_of<Aut>
  {
    detail::evaluator<Aut> e(a);
    return e(w);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename LabelSet>
      weight
      eval(const automaton& aut, const label& lbl)
      {
        const auto& a = aut->as<Aut>();
        const auto& l = lbl->as<LabelSet>().label();
        auto res = ::vcsn::eval(a, l);
        const auto& ctx = a->context();
        return make_weight(*ctx.weightset(), res);
      }

      REGISTER_DECLARE(eval,
                       (const automaton& aut, const label& s) -> weight);
    }
  }
} // namespace vcsn
