#ifndef VCSN_ALGOS_IS_NORMALIZED_HH
# define VCSN_ALGOS_IS_NORMALIZED_HH

# include <vcsn/dyn/algos.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  template <typename Aut>
  bool
  is_normalized(const Aut& a)
  {
    return
      a->num_initials() == 1
      && a->weightset()->is_one(a->weight_of(a->initial_transitions().front()))
      && a->num_finals() == 1
      && a->weightset()->is_one(a->weight_of(a->final_transitions().front()))
      // No arrival on the initial state.
      && a->in(a->dst_of(a->initial_transitions().front())).empty()
      // No departure from the final state.
      && a->out(a->src_of(a->final_transitions().front())).empty();
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      bool
      is_normalized(const automaton& aut)
      {
        return is_normalized(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_normalized,
                       (const automaton& aut) -> bool);
    }
  }
} // vcsn::

#endif // !VCSN_ALGOS_IS_NORMALIZED_HH
