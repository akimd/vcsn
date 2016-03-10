#pragma once

#include <vcsn/algos/copy.hh>

namespace vcsn
{
  template <Automaton Aut>
  Aut
  conjugate(const Aut& aut)
  {
    require(aut->labelset()->has_one(),
            __func__, ": labelset must be nullable");

    const auto& ls = *aut->labelset();
    const auto& ws = *aut->weightset();

    auto res = make_shared_ptr<Aut>(aut->context());
    auto copy_pref = make_copier(aut, res);
    auto copy_suff = make_copier(aut, res);

    // Copy initial automaton.
    copy_pref();
    for (const auto s: aut->states())
      {
        copy_pref([] (state_t_of<Aut> s) { return true; },
                  // Remove all initial and final transitions.
                  [aut] (transition_t_of<Aut> t) {
                    return (aut->src_of(t) != aut->pre()
                            && aut->dst_of(t) != aut->post()); });
        copy_suff([] (state_t_of<Aut> s) { return true; },
                  // Remove all initial and final transitions.
                  [aut] (transition_t_of<Aut> t) {
                    return (aut->src_of(t) != aut->pre())
                      && (aut->dst_of(t) != aut->post()); });

        res->set_initial(copy_pref.state_map().at(s));
        res->set_final(copy_suff.state_map().at(s));

        // Add transitions from all final states of pref to all
        // initial states of suff.
        for (const auto ft: final_transitions(aut))
          for (const auto it: initial_transitions(aut))
            res->new_transition(copy_pref.state_map().at(aut->src_of(ft)),
                                copy_suff.state_map().at(aut->dst_of(it)),
                                ls.one(), ws.one());
      }

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      conjugate(const automaton& a)
      {
        const auto& aut = a->as<Aut>();
        return conjugate(aut);
      }
    }
  }
}
