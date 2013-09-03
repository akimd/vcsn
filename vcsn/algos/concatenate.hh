#ifndef VCSN_ALGOS_CONCATENATE_HH
# define VCSN_ALGOS_CONCATENATE_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

# include <map>

namespace vcsn
{
  /*-------------.
  | concatenate  |
  `-------------*/

  template <class A, class B>
  A
  concatenate(const A& laut, const B& raut)
  {
    // concatenate only works on standard automata.
    assert(is_standard(laut) && is_standard(raut));
    using automaton_t = A;
    using context_t = typename automaton_t::context_t;
    using state_t = typename automaton_t::state_t;
    using weightset_t = typename context_t::weightset_t;
    std::map<state_t, state_t> states_r;
    std::map<state_t, state_t> states_l;
    std::map<state_t, state_t> finals_l;

    // Create new automata.
    auto gs = get_union(*laut.context().labelset(),
                        *raut.context().labelset());
    auto ls = std::make_shared<typename automaton_t::labelset_t>
                              (*laut.context().labelset());
    auto ctx = context_t{ls, laut.context().weightset()};
    automaton_t res(ctx);

    // Add laut states.
    for (auto t: laut.states())
      states_l.emplace(t, res.new_state());
    // Add raut states.
    for (auto t: raut.states())
      if (!raut.is_initial(t))
        states_r.emplace(t, res.new_state());

    // Add laut.
    for (auto t: laut.transitions())
      res.add_transition(states_l[laut.src_of(t)], states_l[laut.dst_of(t)],
                         laut.label_of(t), laut.weight_of(t));
    for (auto t: laut.initial_transitions())
      res.add_initial(states_l[laut.dst_of(t)], laut.weight_of(t));

    // Add raut.
    // Laut finals states fuse with raut initial states.
    weightset_t ws(*ctx.weightset());
    for (auto r: raut.transitions())
      if (raut.is_initial(raut.src_of(r)))
        for (auto l: laut.final_transitions())
          res.add_transition(states_l[laut.src_of(l)], states_r[raut.dst_of(r)],
                             raut.label_of(r),
                             ws.mul(raut.weight_of(r), laut.weight_of(l)));
      else
        res.add_transition(states_r[raut.src_of(r)], states_r[raut.dst_of(r)],
                           raut.label_of(r), raut.weight_of(r));
    for (auto r: raut.final_transitions())
      if (raut.is_initial(raut.src_of(r)))
        for (auto l: laut.final_transitions())
          res.add_final(states_l[laut.src_of(l)],
                        ws.mul(raut.weight_of(r), laut.weight_of(l)));
      else
        res.add_final(states_r[raut.src_of(r)], raut.weight_of(r));

    return res;
  }

  namespace dyn
  {

    namespace detail
    {
      /*-------------------.
      | dyn::concatenate.  |
      `-------------------*/

      template <typename Lhs, typename Rhs>
      automaton
      concatenate(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(l.context(), concatenate(l, r));
      }

      REGISTER_DECLARE2(concatenate,
                        (const automaton&, const automaton&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_CONCATENATE_HH

