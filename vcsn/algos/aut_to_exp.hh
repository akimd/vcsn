#ifndef VCSN_ALGOS_AUT_TO_EXP_HH
# define VCSN_ALGOS_AUT_TO_EXP_HH

#include <vcsn/misc/echo.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{

  /*----------------.
  | state_chooser.  |
  `----------------*/

  /// A state (inner) from an automaton.
  template <typename Aut,
            typename Lifted = detail::lifted_automaton_t<Aut>>
  using state_chooser_t =
    std::function<typename Lifted::state_t(const Lifted&)>;


  /*--------------------------.
  | Naive heuristics degree.  |
  `--------------------------*/

  template <class Aut>
  typename Aut::state_t
  next_naive(const Aut& a)
  {
    typename Aut::state_t best = 0;
    bool best_has_loop = false;
    size_t best_degree = std::numeric_limits<size_t>::max();
    for (auto s: a.states())
      {
        size_t out = 0;
        // Since we are in LAU, there can be at most one such loop.
        bool has_loop = false;
        // Don't count the loops as out-degree.
        for (auto t: a.all_out(s))
          if (a.dst_of(t) != s)
            ++out;
          else
            has_loop = true;
        size_t in = a.all_in(s).size();
        size_t degree = in * out;
        // We prefer to delete a state that has no loop transition.
        if (degree < best_degree
            || (degree == best_degree && has_loop < best_has_loop))
          {
            best = s;
            best_degree = degree;
            best_has_loop = has_loop;
            SHOW(V(best) << V(best_degree) << V(best_has_loop));
          }
      }
    assert(best);
    return best;
  }

  /*------------------.
  | eliminate_state.  |
  `------------------*/

  template <typename Aut>
  void
  eliminate_state(Aut& aut,
                  typename Aut::state_t s)
  {
    static_assert(Aut::context_t::is_lau,
                  "requires labels_are_one");
    if (!aut.has_state(s))
      throw std::runtime_error("not a valid state: " + std::to_string(s));

    // The loop's weight.
    auto loops = aut.outin(s, s);
    assert(loops.size() == 0 || loops.size() == 1);
    auto w =
      loops.empty() ? aut.weightset()->one()
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
  /*-------------.
  | aut_to_exp.  |
  `-------------*/

  template <typename Aut,
            typename Context = typename Aut::context_t>
  typename Context::ratexp_t
  aut_to_exp(const Aut& a,
             const state_chooser_t<Aut>& next_state)
  {
    // State elimination is performed on the lifted automaton.
    auto aut = lift(a);
    while (aut.num_states())
      eliminate_state(aut, next_state(aut));
    return aut.get_initial_weight(aut.post());
  }


  template <class Aut,
            typename Context = typename Aut::context_t>
  typename Context::ratexp_t
  aut_to_exp_naive(const Aut& a)
  {
    state_chooser_t<Aut> next = next_naive<detail::lifted_automaton_t<Aut>>;
    return aut_to_exp(a, next);
  }

  /*------------------.
  | dyn::aut_to_exp.  |
  `------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      ratexp
      aut_to_exp(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_ratexp(a.context(), aut_to_exp_naive(a));
      }

      REGISTER_DECLARE(aut_to_exp,
                       (const automaton& aut) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_AUT_TO_EXP_HH
