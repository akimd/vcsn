#pragma once

#include <vector>

#include <vcsn/algos/copy.hh> // make_fresh_automaton
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/labelset/tupleset.hh>

namespace vcsn
{
  namespace detail
  {
    // Helper struct to compute the context of the partial identity
    template <Automaton Aut>
    struct partial_identity_context
    {
      using in_labelset_t = labelset_t_of<Aut>;
      using weightset_t = weightset_t_of<Aut>;

      using labelset_t = tupleset<in_labelset_t, in_labelset_t>;
      using context_t = context<labelset_t, weightset_t>;

      static context_t value(const Aut& aut)
      {
        return {labelset_t{*aut->labelset(), *aut->labelset()},
                *aut->weightset()};
      }
    };
  }

  template <Automaton Aut>
  using partial_identity_context_t_of
    = typename detail::partial_identity_context<Aut>::context_t;

  /// Create a partial identity transducer from \a aut
  template <Automaton Aut>
  fresh_automaton_t_of<Aut, partial_identity_context_t_of<Aut>>
  partial_identity(const Aut& aut)
  {
    using part_id_ctx = detail::partial_identity_context<Aut>;
    using automaton_t
      = fresh_automaton_t_of<Aut, typename part_id_ctx::context_t>;
    using state_t = state_t_of<automaton_t>;
    using label_t = label_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(part_id_ctx::value(aut));


    // map from aut state -> res state
    auto map = std::vector<state_t>(states_size(aut));

    map[aut->pre()] = res->pre();
    map[aut->post()] = res->post();

    // Copy every state
    for (auto st : aut->states())
      map[st] = res->new_state();

    // Transform every transition into identity
    for (auto tr : all_transitions(aut))
      res->new_transition(map[aut->src_of(tr)],
                          map[aut->dst_of(tr)],
                          label_t{aut->label_of(tr), aut->label_of(tr)},
                          aut->weight_of(tr));

    return res;
  }

  /*------------------------.
  | dyn::partial_identity.  |
  `------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      partial_identity(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::partial_identity(a);
      }
    }
  }
}
