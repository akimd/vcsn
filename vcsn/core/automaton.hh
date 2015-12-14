#pragma once

#include <vcsn/ctx/traits.hh> // state_t_of

namespace vcsn
{
  namespace detail
  {
    /// Indexes of visible transitions leaving state \a s.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto out(const Aut& aut, state_t_of<Aut> s)
    {
      return aut->all_out(s,
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
      return aut->all_out(s,
                          [&aut,l](transition_t_of<Aut> t)
                          {
                            return aut->labelset()->equal(aut->label_of(t), l);
                          });
    }

    /// Indexes of visible transitions arriving to state \a s.
    ///
    /// Invalidated by del_transition() and del_state().
    template <typename Aut>
    auto in(const Aut& aut, state_t_of<Aut> s)
    {
      return aut->all_in(s,
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
      return aut->all_in(s,
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
  }
}
