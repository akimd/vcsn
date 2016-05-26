#pragma once

#include <unordered_set>

#include <vcsn/algos/transpose.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /// Whether state \a s is deterministic in \a aut.
  template <Automaton Aut>
  bool
  is_deterministic(const Aut& aut, state_t_of<Aut> s)
  {
    using automaton_t = Aut;
    static_assert(labelset_t_of<automaton_t>::is_free(),
                  "is_deterministic: requires free labelset");

    using label_t = label_t_of<automaton_t>;
    auto seen = std::unordered_set<label_t>{};
    for (auto t : all_out(aut, s))
      if (!seen.insert(aut->label_of(t)).second)
        return false;
    return true;
  }

  /// Number of non-deterministic states.
  template <Automaton Aut>
  size_t
  num_deterministic_states(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "num_deterministic_states: requires free labelset");

    size_t res = 0;
    for (auto s: aut->states())
      res += is_deterministic(aut, s);
    return res;
  }

  /// Number of non-deterministic states of transposed automaton.
  template <Automaton Aut>
  size_t
  num_codeterministic_states(const Aut& aut)
  {
    return num_deterministic_states(transpose(aut));
  }

  /// Whether has at most an initial state, and all its states
  /// are deterministic.
  template <Automaton Aut>
  bool
  is_deterministic(const Aut& aut)
  {
    static_assert(labelset_t_of<Aut>::is_free(),
                  "is_deterministic: requires free labelset");

    if (1 < initial_transitions(aut).size())
      return false;

    for (auto s: aut->states())
      if (!is_deterministic(aut, s))
        return false;
    return true;
  }

  /// Whether the transposed automaton is deterministic.
  template <Automaton Aut>
  bool
  is_codeterministic(const Aut& aut)
  {
    return is_deterministic(transpose(aut));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool
      is_deterministic(const automaton& aut)
      {
        return is_deterministic(aut->as<Aut>());
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_codeterministic(const automaton& aut)
      {
        return is_codeterministic(aut->as<Aut>());
      }
    }
  }
} // namespace vscn
