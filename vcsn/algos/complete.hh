#ifndef VCSN_ALGOS_COMPLETE_HH
# define VCSN_ALGOS_COMPLETE_HH

# include <queue>
# include <unordered_map>

# include <vcsn/algos/copy.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/unordered_set.hh>

namespace vcsn
{
  /// Complete \a aut and return it.
  template <typename Aut>
  Aut&
  complete_here(Aut& aut)
  {
    static_assert(Aut::context_t::is_lal, "requires labels_are_letters");

    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using label_t = typename automaton_t::label_t;

    state_t sink_state = aut.new_state();
    bool is_accessible = false; // is sink_state accessible ?

    const auto& ls = *aut.labelset();

    if (aut.num_initials() == 0)
      {
        aut.set_initial(sink_state);
        is_accessible = true;
      }

    /// The outgoing labels of a state.
    std::unordered_set<label_t> labels_met;
    for (auto st : aut.states())
      if (st != sink_state)
        {
          for (auto tr : aut.out(st))
            labels_met.insert(aut.label_of(tr));

          for (auto letter : ls)
            if (!has(labels_met, letter))
              {
                aut.new_transition(st, sink_state, letter);
                is_accessible = true;
              }

          labels_met.clear();
        }

    if (is_accessible)
      for (auto letter : ls)
        aut.new_transition(sink_state, sink_state, letter);
    else
      aut.del_state(sink_state);

    return aut;
  }

  template <typename Aut>
  Aut
  complete(const Aut& aut)
  {
    auto res = ::vcsn::copy(aut);
    complete_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      complete(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(complete(a));
      }

      REGISTER_DECLARE(complete,
                       (const automaton& aut) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_COMPLETE_HH
