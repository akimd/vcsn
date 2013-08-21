#ifndef VCSN_ALGOS_LEFT_MULT_HH
# define VCSN_ALGOS_LEFT_MULT_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/accessible.hh> // dyn::make_automaton

namespace vcsn
{
  /*-----------.
  | left-mult  |
  `-----------*/

  template <class Aut>
  Aut
  left_mult(const Aut& aut, std::string w)
  {
    // left_mult only works on standard automata.
    assert(is_standard(aut));
    using automaton_t = Aut;
    using context_t = typename automaton_t::context_t;
    using weightset_t = typename context_t::weightset_t;
    using value_t = typename weightset_t::value_t;

    auto ls = std::make_shared<typename automaton_t::labelset_t>
                              (*aut.context().labelset());
    auto ctx = context_t{ls, aut.context().weightset()};
    automaton_t res(ctx);

    // Add aut states.
    for (int i = 0; i < aut.num_states(); ++i)
      res.new_state();

    // Only one initial.
    for (auto t: aut.initial_transitions())
      res.add_initial(aut.dst_of(t), aut.weight_of(t));

    // Add finals.
    weightset_t ws(*ctx.weightset());
    // Multiply aut by k.
    value_t k = ws.conv(w);
    for (auto t: aut.final_transitions())
    {
      if (aut.is_initial(aut.src_of(t)))
        res.add_final(aut.src_of(t), ws.mul(k, aut.weight_of(t)));
      else
        res.add_final(aut.src_of(t), aut.weight_of(t));
    }

    // Add transitions.
    for (auto t: aut.transitions())
    {
      if (aut.is_initial(aut.src_of(t)))
        res.add_transition(aut.src_of(t), aut.dst_of(t), aut.label_of(t),
                           ws.mul(k, aut.weight_of(t)));
      else
        res.add_transition(aut.src_of(t), aut.dst_of(t), aut.label_of(t),
                           aut.weight_of(t));
    }

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /*-----------------.
      | dyn::left_mult.  |
      `-----------------*/

      template <typename Aut>
      automaton
      left_mult(const automaton& a, std::string w)
      {
        const auto& aut = dynamic_cast<const Aut&>(*a);
        return make_automaton(aut.context(), left_mult(aut, w));
      }

      REGISTER_DECLARE(left_mult,
                       (const automaton& aut, std::string w) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_LEFT_MULT_HH
