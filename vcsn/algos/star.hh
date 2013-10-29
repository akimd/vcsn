#ifndef VCSN_ALGOS_STAR_HH
# define VCSN_ALGOS_STAR_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

# include <vector>

namespace vcsn
{
  /*------.
  | star  |
  `------*/

  /// In place star of a standard automaton.
  ///
  /// See standard_visitor::visit(star).
  template <class Aut>
  Aut&
  star_here(Aut& res)
  {
    assert(is_standard(res));

    using automaton_t = Aut;
    using context_t = typename automaton_t::context_t;
    using weightset_t = typename context_t::weightset_t;
    using weight_t = typename context_t::weight_t;
    using state_t = typename automaton_t::state_t;

    weightset_t ws(*res.context().weightset());

    state_t initial = res.dst_of(res.initial_transitions().front());
    // The "final weight of the initial state", starred.
    weight_t w = ws.star(res.get_final_weight(initial));
    // Branch all the final states (but initial) to the successors
    // of initial.
    for (auto ti: res.out(initial))
      {
        res.lmul_weight(ti, w);
        for (auto tf: res.final_transitions())
          if (res.src_of(tf) != initial)
            // The weight of ti has already been multiplied, on the
            // left, by w.
            res.add_transition
              (res.src_of(tf),
               res.dst_of(ti),
               res.label_of(ti),
               ws.mul(res.weight_of(tf), res.weight_of(ti)));
      }
    for (auto tf: res.final_transitions())
      res.rmul_weight(tf, w);
    res.set_final(initial, w);
    return res;
  }


  /// Star of a standard automaton.
  template <class Aut>
  Aut
  star(const Aut& aut)
  {
    auto res = copy(aut);
    star_here(res);
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
        const auto& aut = a->as<Aut>();
        return make_automaton(star(aut));
      }

      REGISTER_DECLARE(star,
                       (const automaton& aut) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_STAR_HH
