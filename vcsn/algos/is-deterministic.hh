#pragma once

#include <unordered_set>

#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton.hh> // all_out
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /// Whether state \a s is deterministic in \a aut.
  template <Automaton Aut>
  bool
  is_deterministic(const Aut& aut, state_t_of<Aut> s)
  {
    const auto& ls = *aut->labelset();
    using automaton_t = Aut;
    using label_t = label_t_of<automaton_t>;
    auto seen = std::unordered_set<label_t>{};
    for (const auto t : all_out(aut, s))
      if (!ls.is_letter(aut->label_of(t))
          || !seen.insert(aut->label_of(t)).second)
        return false;
    return true;
  }

  /// Number of deterministic states.
  template <Automaton Aut>
  size_t
  num_deterministic_states(const Aut& aut)
  {
    size_t res = 0;
    for (auto s: aut->states())
      res += is_deterministic(aut, s);
    return res;
  }

  /// Number of deterministic states of transposed automaton.
  template <Automaton Aut>
  size_t
  num_codeterministic_states(const Aut& aut)
  {
    return num_deterministic_states(transpose(aut));
  }

  namespace detail
  {
    template <Automaton Aut>
    bool
    is_deterministic_(const Aut& aut)
    {
      if (1 < initial_transitions(aut).size())
        return false;

      for (auto s: aut->states())
        if (!is_deterministic(aut, s))
          return false;
      return true;
    }
  }

  /// Whether has at most an initial state, and all its states
  /// are deterministic.
  template <Automaton Aut>
  bool
  is_deterministic(const Aut& aut)
  {
    if (aut->properties().is_unknown(*aut, is_deterministic_ptag{}))
    {
      auto res = detail::is_deterministic_(aut);
      aut->properties().put(*aut, is_deterministic_ptag{}, res);
      return res;
    }
    else
      return aut->properties().get(*aut, is_deterministic_ptag{});
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
        return is_deterministic(aut->template as<Aut>());
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_codeterministic(const automaton& aut)
      {
        return is_codeterministic(aut->template as<Aut>());
      }
    }
  }
} // namespace vscn
