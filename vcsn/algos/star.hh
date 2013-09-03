#ifndef VCSN_ALGOS_STAR_HH
# define VCSN_ALGOS_STAR_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

# include <vector>

namespace vcsn
{
  /*------.
  | star  |
  `------*/

  /// Star of a standard automaton.
  template <class Aut>
  Aut
  star(const Aut& aut)
  {
    // star only works on standard automata.
    assert(is_standard(aut));
    using automaton_t = Aut;
    using context_t = typename automaton_t::context_t;
    using state_t = typename automaton_t::state_t;
    using transition_t = typename automaton_t::transition_t;
    using weightset_t = typename context_t::weightset_t;

    auto ls = std::make_shared<typename automaton_t::labelset_t>
                              (*aut.context().labelset());
    auto ctx = context_t{ls, aut.context().weightset()};
    automaton_t res(ctx);

    // State in aut -> state in Res.
    std::map<state_t, state_t> m;
    for (auto t: aut.states())
      m.emplace(t, res.new_state());
    m.emplace(aut.pre(), res.pre());
    m.emplace(aut.post(), res.post());

    // Only one initial.
    state_t initial = aut.dst_of(aut.initial_transitions().front());
    res.add_initial(m[initial], aut.get_initial_weight(initial));

    // Add finals.
    weightset_t ws(*ctx.weightset());
    for (auto t: aut.final_transitions())
      if (!aut.is_initial(aut.src_of(t)))
        res.add_final(m[aut.src_of(t)],
                      ws.mul(aut.weight_of(t),
                             ws.star(aut.get_final_weight(initial))));
    res.add_final(initial, ws.star(aut.get_final_weight(initial)));

    // Memorize transitions from initial state.
    std::vector<transition_t> succ;
    // Add transitions.
    // Add existing transitions with new weight.
    for (auto t: aut.transitions())
      if (aut.is_initial(aut.src_of(t)))
      {
        succ.emplace_back(t);
        res.add_transition(m[aut.src_of(t)], m[aut.dst_of(t)],
                           aut.label_of(t),
                           ws.mul(ws.star(aut.get_final_weight(initial)),
                                  aut.weight_of(t)));
      }
      else
        res.add_transition(m[aut.src_of(t)], m[aut.dst_of(t)],
                           aut.label_of(t), aut.weight_of(t));

    // Link finals to successor of initial state.
    for (auto t: aut.final_transitions())
      if (!aut.is_initial(aut.src_of(t)))
        for (auto s: succ)
          res.add_transition(m[aut.src_of(t)], m[aut.dst_of(s)],
                             aut.label_of(s),
                             ws.mul(aut.weight_of(t),
                                    ws.mul(ws.star(aut.get_final_weight(initial)),
                                           aut.weight_of(s))));

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /*------------.
      | dyn::star.  |
      `------------*/

      template <typename Aut>
      automaton
      star(const automaton& a)
      {
        const auto& aut = dynamic_cast<const Aut&>(*a);
        return make_automaton(aut.context(), star(aut));
      }

      REGISTER_DECLARE(star,
                       (const automaton& aut) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_STAR_HH
