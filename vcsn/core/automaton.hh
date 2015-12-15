#pragma once

#include <vcsn/ctx/traits.hh> // state_t_of

namespace vcsn
{
  namespace detail
  {
    /// Indexes of transitions leaving state \a s that validate the
    /// predicate.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut, typename Pred>
    auto all_out(const Aut& aut, state_t_of<Aut> s, Pred pred)
    {
      return make_container_filter_range(aut->all_out(s), pred);
    }

    /// Indexes of visible transitions leaving state \a s.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto out(const Aut& aut, state_t_of<Aut> s)
    {
      return all_out(aut, s,
                     [&aut](transition_t_of<Aut> t)
                     {
                       return aut->dst_of(t) != aut->post();
                     });
    }

    /// Indexes of all transitions leaving state \a s on label \a l.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto out(const Aut& aut, state_t_of<Aut> s, label_t_of<Aut> l)
    {
      return all_out(aut, s,
                     [&aut,l](transition_t_of<Aut> t)
                     {
                       return aut->labelset()->equal(aut->label_of(t), l);
                     });
    }


    /// Indexes of transitions entering state \a s that validate the
    /// predicate.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut, typename Pred>
    auto all_in(const Aut& aut, state_t_of<Aut> s, Pred pred)
    {
      return make_container_filter_range(aut->all_in(s), pred);
    }

    /// Indexes of visible transitions arriving to state \a s.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto in(const Aut& aut, state_t_of<Aut> s)
    {
      return all_in(aut, s,
                    [&aut](transition_t_of<Aut> t)
                    {
                      return aut->src_of(t) != aut->pre();
                    });
    }

    /// Indexes of visible transitions arriving to state \a s on label \a l.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto in(const Aut& aut, state_t_of<Aut> s, label_t_of<Aut> l)
    {
      return all_in(aut, s,
                    [&aut,l](transition_t_of<Aut> t)
                    {
                      return aut->labelset()->equal(aut->label_of(t), l);
                    });
    }


    /// Indexes of transitions to (visible) initial states.
    ///
    /// Also include the weird case of a transition from pre to post.
    /// This is used when calling eliminate_state repeatedly.
    template <typename Aut>
    auto initial_transitions(const Aut& aut)
      -> decltype(aut->all_out(aut->pre())) // for G++ 5.2
    {
      return aut->all_out(aut->pre());
    }

    /// Indexes of transitions from (visible) final states.
    ///
    /// Also include the weird case of a transition from pre to post.
    /// This is used when calling eliminate_state repeatedly.
    template <typename Aut>
    auto final_transitions(const Aut& aut)
      -> decltype(aut->all_in(aut->post())) // for G++ 5.2
    {
      return aut->all_in(aut->post());
    }


    /// Indexes of visible transitions from state \a s to state \a d.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto outin(const Aut& aut, state_t_of<Aut> s, state_t_of<Aut> d)
    {
      return all_out(aut, s,
                     [aut, d](transition_t_of<Aut> t)
                     {
                       return aut->dst_of(t) == d;
                     });
    }

    /// Remove all the transitions between s and d.
    template <typename Aut>
    void del_transition(const Aut& aut, state_t_of<Aut> s, state_t_of<Aut> d)
    {
      // Make a copy of the transition indexes, as the iterators are
      // invalidated by del_transition(t).
      for (auto t: make_vector(outin(aut, s, d)))
        aut->del_transition(t);
    }
  }
}
