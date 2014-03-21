#ifndef SYNCHRONIZING_WORD_HH
# define SYNCHRONIZING_WORD_HH

# include <vector>
# include <map>
# include <set>
# include <queue>
# include <iostream>

# include <vcsn/algos/copy.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/misc/pair.hh>

namespace vcsn
{
  template <typename T>
  std::pair<T, T> make_ordered_pair(T e1, T e2)
  {
    return (e1 > e2) ? std::make_pair(e2, e1) : std::make_pair(e1, e2);
  }

  template <typename Aut>
  Aut pair(const Aut& aut)
  {
    using context_t = typename Aut::context_t;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;
    using weightset_t = typename automaton_t::weightset_t;
    using weight_t = typename weightset_t::value_t;
    using pair_t = std::pair<state_t, state_t>;

    auto ctx = aut.context();
    auto ws = ctx.weightset();

    std::unordered_map<pair_t, state_t> pair_states;

    automaton_t res(ctx);
    state_t q0 = res.new_state(); // q0 special state

    for (auto s1: aut.states())
      for (auto s2: aut.states())
        if (s1 != s2)
          {
            auto np = make_ordered_pair(s1, s2);
            if (pair_states.find(np) == pair_states.end())
              pair_states[np] = res.new_state();
          }

    for (auto ps : pair_states)
      {
        auto pstates = ps.first; // pair of states
        auto cstate = ps.second; // current state

        for (auto t1: aut.out(pstates.first))
          {
            auto label = aut.label_of(t1);
            auto dst1 = aut.dst_of(t1);
            for (auto t2: aut.out(pstates.second, label))
              {
                auto dst2 = aut.dst_of(t2);
                weight_t nw = ws->mul(aut.weight_of(t1), aut.weight_of(t2));
                if (dst1 != dst2)
                  {
                    auto np = make_ordered_pair(dst1, dst2);
                    res.add_transition(cstate, pair_states.find(np)->second,
                                       label, nw);
                  }
                else
                    res.add_transition(cstate, q0, label, nw);
              }
          }
      }

    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      pair(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(pair(a));
      }

      REGISTER_DECLARE(pair, (const automaton&) -> automaton);
    }
  }
}

#endif /* !SYNCHRONIZING_WORD_HH */
