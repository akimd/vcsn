#ifndef VCSN_ALGOS_AUT_TO_EXP_HH
# define VCSN_ALGOS_AUT_TO_EXP_HH

#include <vcsn/misc/echo.hh>
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
        SHOW(V(s));

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

  /*----------------.
  | next in order.  |
  `----------------*/

  template <class Aut>
  typename Aut::state_t
  next_in_order(const Aut& a)
  {
    return a.states().front();
  }

  template <class Aut,
            typename Context = typename Aut::context_t>
  typename Context::kratexp_t
  aut_to_exp(const Aut& a)
  {
    state_chooser_t<Aut> next =
      next_in_order<details::lifted_automaton_t<Aut>>;
    return aut_to_exp(a, next);
  }

  /*----------------------.
  | abstract aut_to_exp.  |
  `----------------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      rat::exp_t
      aut_to_exp(const dyn::abstract_automaton& aut)
      {
        return ::vcsn::aut_to_exp(dynamic_cast<const Aut&>(aut));
      }

      using aut_to_exp_t =
        auto (const dyn::abstract_automaton& aut) -> rat::exp_t;

      bool aut_to_exp_register(const std::string& ctx, const aut_to_exp_t& fn);
    }
  }


  /*-----------------.
  | Highest degree.  |
  `-----------------*/

  template <class Aut>
  typename Aut::state_t
  next_in_degree(const Aut& a)
  {
    typename Aut::state_t best = 0;
    size_t loops_best = std::numeric_limits<size_t>::max();
    size_t degree_best = 0;
    for (auto s: a.states())
      {
        size_t in = 0;
        size_t loops = 0;
        // Don't count the loops as out-degree.
        for (auto t: a.all_out(s))
          if (a.dst_of(t) != s)
            ++in;
          else
            ++loops;
        size_t out = a.all_in(s).size() - loops;
        size_t degree = in * out;
        // We prefer to delete a state that has no loop transition.
        if (degree_best < degree
            || (degree_best == degree && loops < loops_best))
          {
            best = s;
            degree_best = degree;
            loops_best = loops;
            SHOW(V(best) << V(degree_best) << V(loops_best));
          }
      }
    assert(best);
    return best;
  }

  template <class Aut,
            typename Context = typename Aut::context_t>
  typename Context::kratexp_t
  aut_to_exp_in_degree(const Aut& a)
  {
    state_chooser_t<Aut> next =
      next_in_degree<details::lifted_automaton_t<Aut>>;
    return aut_to_exp(a, next);
  }

  /*--------------------------------.
  | abstract aut_to_exp_in_degree.  |
  `--------------------------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      rat::exp_t
      aut_to_exp_in_degree(const dyn::abstract_automaton& aut)
      {
        return ::vcsn::aut_to_exp_in_degree(dynamic_cast<const Aut&>(aut));
      }

      bool aut_to_exp_in_degree_register(const std::string& ctx,
                                         const aut_to_exp_t& fn);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_AUT_TO_EXP_HH
