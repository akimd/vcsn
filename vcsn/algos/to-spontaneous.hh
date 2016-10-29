#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/misc/type_traits.hh>
#include <vcsn/labelset/oneset.hh>

namespace vcsn
{
  /// Convert to spontaneous automaton: change every transition's
  /// label to the empty word.
  template <Automaton Aut>
  auto
  to_spontaneous(const Aut& aut)
    -> std::enable_if_t<!context_t_of<Aut>::is_lao,
                         decltype(make_mutable_automaton(make_context(oneset(),
                                                                      *aut->weightset())))>
  {
    auto res
      = make_mutable_automaton(make_context(oneset(), *aut->weightset()));

    using in_state_t = state_t_of<Aut>;
    using out_state_t = state_t_of<decltype(res)>;
    /// input state -> output state.
    using state_map_t = std::unordered_map<in_state_t, out_state_t>;
    auto out_state = state_map_t{{aut->pre(),  res->pre()},
                                 {aut->post(), res->post()}};
    for (auto s: aut->states())
      out_state[s] = res->new_state();

    const auto& ils = *aut->labelset();
    const auto& ols = *res->labelset();

    for (auto t : all_transitions(aut))
      res->add_transition(out_state[aut->src_of(t)],
                          out_state[aut->dst_of(t)],
                          ils.is_special(aut->label_of(t))
                          ? ols.special() : ols.one(),
                          aut->weight_of(t));
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
