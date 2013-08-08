#ifndef VCSN_ALGOS_COMPLETE_HH
# define VCSN_ALGOS_COMPLETE_HH

# include <queue>
# include <unordered_map>

# include <vcsn/misc/unordered_set.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  template <class Aut>
  Aut
  complete (const Aut& aut)
  {
    static_assert(Aut::context_t::is_lal, "requires labels_are_letters");

    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using label_t = typename automaton_t::label_t;

    std::unordered_map<state_t, state_t> states_assoc;

    automaton_t new_aut{aut.context()};

    states_assoc[aut.pre()] = new_aut.pre();
    states_assoc[aut.post()] = new_aut.post();

    for (auto st : aut.states())
      {
        states_assoc[st] = new_aut.new_state();
        if (aut.is_initial(st))
          new_aut.set_initial(states_assoc[st]);
        if (aut.is_final(st))
          new_aut.set_final(states_assoc[st]);
      }

    for (auto st : aut.states())
      for (auto tr : aut.out(st))
        new_aut.add_transition(states_assoc[st], states_assoc[aut.dst_of(tr)],
                               aut.label_of(tr), aut.weight_of(tr));

    state_t sink_state = new_aut.new_state();
    bool is_accessible = false; // is new_state accessible ?
    std::unordered_set<label_t> labels_met;

    auto letters = *new_aut.labelset();

    if (aut.num_initials() == 0)
      {
        new_aut.set_initial(sink_state);
        is_accessible = true;
      }

    for (auto st : new_aut.states())
      {
        if (st == sink_state)
          continue;

        for (auto tr : new_aut.out(st))
          labels_met.insert(new_aut.label_of(tr));

        for (auto letter : letters)
          if (!has(labels_met, letter))
            {
              new_aut.add_transition(st, sink_state, letter);
              is_accessible = true;
            }

        labels_met.clear();
      }

    if (is_accessible)
      for (auto letter : letters)
        new_aut.add_transition(sink_state, sink_state, letter);
    else
      new_aut.del_state(sink_state);

    return new_aut;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      complete(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(a.context(), complete(a));
      }

      REGISTER_DECLARE(complete,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_COMPLETE_HH
