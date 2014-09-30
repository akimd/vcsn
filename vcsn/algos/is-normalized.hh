#ifndef VCSN_ALGOS_IS_NORMALIZED_HH
# define VCSN_ALGOS_IS_NORMALIZED_HH

# include <vcsn/dyn/algos.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /// Whether \a a is standard and co-standard.
  template <typename Aut>
  bool
  is_normalized(const Aut& a)
  {
    auto inis = a->initial_transitions();
    auto fins = a->final_transitions();
    return
      inis.size() == 1
      && a->weightset()->is_one(a->weight_of(inis.front()))
      && fins.size() == 1
      && a->weightset()->is_one(a->weight_of(fins.front()))
      // No arrival on the initial state.
      && a->in(a->dst_of(inis.front())).empty()
      // No departure from the final state.
      && a->out(a->src_of(fins.front())).empty();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
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
