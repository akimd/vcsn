#ifndef VCSN_ALGOS_UNION_HH
# define VCSN_ALGOS_UNION_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/accessible.hh> // dyn::make_automaton

namespace vcsn
{
  /*----------.
  | union_a.  |
  `----------*/

  template <class A, class B>
  A
  union_a(const A& laut, const B& raut)
  {
    using automaton_t = A;
    using context_t = typename automaton_t::context_t;
    using state_t = typename automaton_t::state_t;
    std::map<state_t, state_t> states_r;
    std::map<state_t, state_t> states_l;

    // Create new automata.
    auto gs = get_union(*laut.context().labelset(),
                        *raut.context().labelset());
    auto ls = std::make_shared<typename automaton_t::labelset_t>(gs);
    auto ctx = context_t{ls, laut.context().weightset()};
    automaton_t res(ctx);

    // Add laut.
    for (auto t: laut.states())
      states_l.emplace(t, res.new_state());
    for (auto t: laut.transitions())
      res.add_transition(states_l[laut.src_of(t)], states_l[laut.dst_of(t)],
                         laut.label_of(t), laut.weight_of(t));
    for (auto t: laut.initial_transitions())
      res.add_initial(states_l[laut.dst_of(t)], laut.weight_of(t));
    for (auto t: laut.final_transitions())
      res.add_final(states_l[laut.src_of(t)], laut.weight_of(t));

    // Add raut
    for (auto t: raut.states())
      states_r.emplace(t, res.new_state());
    for (auto t: raut.transitions())
      res.add_transition(states_r[raut.src_of(t)], states_r[raut.dst_of(t)],
                         raut.label_of(t), raut.weight_of(t));
    for (auto t: raut.final_transitions())
      res.add_final(states_r[raut.src_of(t)], raut.weight_of(t));
    for (auto t: raut.initial_transitions())
      res.add_initial(states_r[raut.dst_of(t)], raut.weight_of(t));
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /*---------------.
      | dyn::union_a.  |
      `---------------*/

      template <typename Lhs, typename Rhs>
      automaton
      union_a(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = dynamic_cast<const Lhs&>(*lhs);
        const auto& r = dynamic_cast<const Lhs&>(*rhs);
        return make_automaton(l.context(), union_a(l, r));
      }

      using union_a_t =
        auto (const automaton& lhs, const automaton& rhs) -> automaton;
      bool union_a_register(const std::string& lctx, const std::string& rctx,
                            union_a_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_UNION_HH
