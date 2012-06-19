#ifndef VCSN_ALGOS_AUT_TO_EXP_HH
# define VCSN_ALGOS_AUT_TO_EXP_HH

# include <vcsn/algos/lift.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/kratexp.hh>

namespace vcsn
{
  /// A state (inner) from an automaton.
  template <typename Aut,
            typename Lifted = details::lifted_automaton_t<Aut>>
  using state_chooser_t =
    std::function<typename Lifted::state_t(const Lifted&)>;

  template <typename Aut,
            typename Context = typename Aut::context_t>
  typename Context::kratexp_t
  aut_to_exp(const Aut& a,
             const state_chooser_t<Aut>& next_state)
  {
    // State elimination is performed on the lifted automaton.
    auto aut = lift(a);
    while (aut.num_states())
      {
        auto s = next_state(aut);

        // The loop's weight.
        auto loops = aut.outin(s, s);
        assert(loops.size() == 0 || loops.size() == 1);
        auto w =
          loops.empty() ? aut.weightset()->unit()
          : aut.weightset()->star(aut.weight_of(loops.front()));

        // Get all the predecessors, and successors.
        auto outs = aut.all_out(s);
        for (auto in: aut.all_in(s))
          for (auto out: outs)
            aut.add_transition
              (aut.src_of(in), aut.dst_of(out),
               aut.label_of(in),
               aut.weightset()->mul(aut.weight_of(in),
                                    aut.weightset()->mul(w,
                                                         aut.weight_of(out))));
        aut.del_state(s);
      }
    return aut.get_initial_weight(aut.post());
  }

  template <class Aut,
            typename Context = typename Aut::context_t>
  std::shared_ptr<const typename Context::node_t>
  aut_to_exp(Aut& a)
  {
    state_chooser_t<Aut> next =
      [] (const details::lifted_automaton_t<Aut>& a)
      { return a.states().front(); };
    return aut_to_exp(a, next);
  }
} // vcsn::

#endif // !VCSN_ALGOS_AUT_TO_EXP_HH
