#ifndef VCSN_ALGOS_SUM_HH
# define VCSN_ALGOS_SUM_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/accessible.hh> // dyn::make_automaton

# include <map>

namespace vcsn
{
  /*-----.
  | sum  |
  `-----*/

  template <class A, class B>
  A
  sum(const A& laut, const B& raut)
  {
    // Sum only works on standard automata.
    assert(is_standard(laut) && is_standard(raut));
    using automaton_t = A;
    using context_t = typename automaton_t::context_t;
    using state_t = typename automaton_t::state_t;
    std::map<state_t, state_t> states_r;
    std::map<state_t, state_t> states_l;

    // Create new automata.
    auto gs = get_union(*laut.context().labelset(),
                           *raut.context().labelset());
    auto ls = std::make_shared<typename automaton_t::labelset_t>
                              (*laut.context().labelset());
    auto ctx = context_t{ls, laut.context().weightset()};
    automaton_t res(ctx);

    // New automaton has only one initial state.
    state_t initial;
    for (auto t: laut.states())
    {
      if (laut.is_initial(t))
        initial = t;
      states_l.emplace(t, res.new_state());
    }
    // Add laut.
    for (auto t: laut.transitions())
      res.add_transition(states_l[laut.src_of(t)], states_l[laut.dst_of(t)],
                         laut.label_of(t), laut.weight_of(t));
    // There is only one initial transition
    for (auto t: laut.initial_transitions())
      res.add_initial(states_l[laut.dst_of(t)], laut.weight_of(t));
    for (auto t: laut.final_transitions())
      res.add_final(states_l[laut.src_of(t)], laut.weight_of(t));

    // Add raut
    for (auto t: raut.states())
    {
      if (!raut.is_initial(t))
        states_r.emplace(t, res.new_state());
      // Fuse raut and laut initial state.
      else
        states_r.emplace(t, initial);
    }
    for (auto t: raut.transitions())
      res.add_transition(states_r[raut.src_of(t)], states_r[raut.dst_of(t)],
                         raut.label_of(t), raut.weight_of(t));
    for (auto t: raut.final_transitions())
      res.add_final(states_r[raut.src_of(t)], raut.weight_of(t));
    return res;
  }

  namespace dyn
  {
    // Create new automata.
    namespace detail
    {
      /*-----------.
      | dyn::sum.  |
      `-----------*/

      template <typename Lhs, typename Rhs>
      automaton
      sum(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = dynamic_cast<const Lhs&>(*lhs);
        const auto& r = dynamic_cast<const Lhs&>(*rhs);
        return make_automaton(l.context(), sum(l, r));
      }

      using sum_t =
        auto (const automaton& lhs, const automaton& rhs) -> automaton;
      bool sum_register(const std::string& lctx, const std::string& rctx,
                            sum_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_SUM_HH
