#ifndef VCSN_ALGOS_EVAL_HH
# define VCSN_ALGOS_EVAL_HH

# include <algorithm>
# include <vector>

# include <vcsn/core/kind.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/weight.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Aut>
    class evaluator
    {
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");

      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using word_t = typename automaton_t::labelset_t::word_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

      // state -> weight.
      using weights_t = std::vector<weight_t>;

    public:
      evaluator(const automaton_t& a)
        : a_(a)
        , ws_(*a_.weightset())
      {}

      weight_t operator()(const word_t& word) const
      {
        // Initialization.
        const weight_t zero = ws_.zero();
        // FIXME: a perfect job for a sparse array: most of the states
        // will be not visited, nevertheless, because we iterate on
        // all the states, they are costly at each iteration.

        /// An array indexed by state numbers.
        const auto& states = a_.states();
        size_t last_state = *std::max_element(std::begin(states),
                                              std::end(states));
        // Do not use braces (v1{size, zero}): the type of zero might
        // result in the compiler believing we are building a vector
        // with two values: a_.num_all_states() and zero.
        weights_t v1(last_state + 1, zero);
        v1[a_.pre()] = ws_.one();
        weights_t v2{v1};

        // Computation.
        for (auto l : a_.labelset()->genset()->delimit(word))
          {
            v2.assign(v2.size(), zero);
            for (size_t s = 0; s < v1.size(); ++s)
              if (!ws_.is_zero(v1[s])) // delete if bench >
                for (auto t : a_.out(s, l))
                  // Introducing a reference to v2[a_.dst_of(tr)] is
                  // tempting, but won't work for std::vector<bool>.
                  // FIXME: Specialize for Boolean?
                  v2[a_.dst_of(t)] =
                    ws_.add(v2[a_.dst_of(t)],
                            ws_.mul(v1[s], a_.weight_of(t)));
            std::swap(v1, v2);
          }
        return v1[a_.post()];
      }
    private:
      const automaton_t& a_;
      const weightset_t& ws_;
    };

  } // namespace detail

  template <typename Aut>
  inline
  auto
  eval(const Aut& a, const typename Aut::labelset_t::word_t& w)
    -> typename Aut::weight_t
  {
    detail::evaluator<Aut> e(a);
    return e(w);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename String>
      auto
      eval(const automaton& aut, const std::string& s)
        -> weight
      {
        const auto& a = aut->as<Aut>();
        auto res = ::vcsn::eval(a, s);
        const auto& ctx = a.context();
        return make_weight(*ctx.weightset(), res);
      }

      REGISTER_DECLARE(eval,
                       (const automaton& aut, const std::string& s) -> weight);
    }
  }


} // namespace vcsn

#endif // !VCSN_ALGOS_EVAL_HH
