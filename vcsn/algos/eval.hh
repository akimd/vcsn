#ifndef VCSN_ALGOS_EVAL_HH
# define VCSN_ALGOS_EVAL_HH

# include <algorithm>
# include <map>
# include <cassert>

//# include <vcsn/core/crange.hh>

namespace vcsn {

  template <class Aut>
  class eval_functor
  {
    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using word_t = typename automaton_t::genset_t::word_t;
    using weightset_t = typename automaton_t::weightset_t;
    using weight_t = typename weightset_t::value_t;
    using weights = std::map<state_t, weight_t>;

  public:
    eval_functor(const automaton_t& a)
    : a_(a)
    , w_(a_.weightset())
    {}

    weight_t operator()(const word_t& word) const
    {
      // Initialize
      const weight_t zero = w_.zero();
      weights v1;
      weights v2;
      for (auto s : a_.states())
        v1.insert(std::make_pair(s, zero));

      for (auto init : a_.initial_transitions())
        v1[a_.dst_of(init)] = w_.unit();

      // Computation
      for (auto l : word)
        {
          for (auto& w : v2)
            w.second = zero;
          for (auto w : v1)
            {
              if (zero != w.second)
                {
                  for (auto t : a_.out(w.first, l))
                    {
                      auto& weight = v2[a_.dst_of(t)];
                      weight = w_.add(weight, w_.mul(w.second, a_.weight_of(t)));
                    }
                }
            }
          std::swap(v1, v2);
        }
      weight_t res = zero;
      for (auto w : v1)
        {
          if (zero != w.second)
            res = w_.add(res, w_.mul(w.second, a_.get_final_weight(w.first)));
        }
      return res;
    }

  private:
    const automaton_t& a_;
    const weightset_t& w_;
  };

  template <class Aut>
  inline
  typename Aut::weight_t
  eval(const Aut& a, const typename Aut::genset_t::word_t& w)
  {
    eval_functor<Aut> evalf(a);
    return evalf(w);
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_EVAL_HH
