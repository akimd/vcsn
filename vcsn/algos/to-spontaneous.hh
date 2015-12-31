#pragma once

#include <vcsn/misc/type_traits.hh>
#include <vcsn/algos/copy.hh>

namespace vcsn
{
  /// to_spontaneous
  ///
  /// Convert to spontaneous automaton: simply change every transition's label
  /// to the empty word.
  template <Automaton Aut>
  auto
  to_spontaneous(const Aut& aut)
    -> std::enable_if_t<!context_t_of<Aut>::is_lao,
                         decltype(make_mutable_automaton(make_context(oneset(),
                                                                      *aut->weightset())))>
  {
    auto res = make_mutable_automaton(make_context(oneset(), *aut->weightset()));

    using in_state_t = state_t_of<Aut>;
    using out_state_t = state_t_of<decltype(res)>;
    /// input state -> output state.
    using state_map_t = std::unordered_map<in_state_t, out_state_t>;
    state_map_t out_state_{{aut->pre(),  res->pre()},
                           {aut->post(), res->post()}};
    for (auto s: aut->states())
      out_state_[s] = res->new_state();

    for (auto t : all_transitions(aut))
      {
        auto src = out_state_.find(aut->src_of(t));
        auto dst = out_state_.find(aut->dst_of(t));
        if (src != out_state_.end() && dst != out_state_.end())
          {
            if (src->second == res->pre() || dst->second == res->post())
              res->new_transition(src->second, dst->second,
                                  res->labelset()->special(), aut->weight_of(t));
            else
              res->new_transition(src->second, dst->second,
                                  res->labelset()->one(), aut->weight_of(t));
          }
      }
    return res;
  }

  /// to_spontaneous specialisation for lao automaton.
  ///
  /// The automaton is already spontaneous: we do not need to convert it.
  template <Automaton Aut>
  auto
  to_spontaneous(const Aut& aut)
    -> std::enable_if_t<context_t_of<Aut>::is_lao, decltype(copy(aut))>
  {
    return copy(aut);
  }
}
