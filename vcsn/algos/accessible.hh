#pragma once

#include <limits>
#include <queue>

#include <vcsn/algos/filter.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/unordered_set.hh>

namespace vcsn
{
  /*----------------.
  | Function tags.  |
  `----------------*/

  CREATE_FUNCTION_TAG(accessible);
  CREATE_FUNCTION_TAG(coaccessible);
  CREATE_FUNCTION_TAG(trim);

  /*--------------------------------------------------.
  | Sets of accessible, coaccessible, useful states.  |
  `--------------------------------------------------*/

  template <Automaton Aut>
  using states_t = std::unordered_set<state_t_of<Aut>>;

  /// The set of accessible states, including pre(), and possibly post().
  ///
  /// \param aut     the automaton.
  /// \param strict  whether to evaluate lazy states.
  template <Automaton Aut>
  states_t<Aut>
  accessible_states(const Aut& aut, bool strict = true)
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;

    // Reachable states.
    auto res = states_t<Aut>{aut->pre()};

    // States work list.
    auto todo = std::queue<state_t>{};
    todo.emplace(aut->pre());

    auto credit
      = getenv("VCSN_CREDIT")
      ? std::stoul(getenv("VCSN_CREDIT"))
      : std::numeric_limits<unsigned>::max();
    while (!todo.empty())
      {
        const state_t src = todo.front();
        todo.pop();

        if (strict || !aut->is_lazy(src))
          for (auto tr : all_out(aut, src))
            {
              state_t dst = aut->dst_of(tr);
              // If we have not seen it already, explore its successors.
              if (res.emplace(dst).second && credit--)
                todo.emplace(dst);
            }
      }

    return res;
  }

  /// The set of coaccessible states, including post(), and possibly pre().
  ///
  /// \param a       the automaton.
  /// \param strict  whether to evaluate lazy states.
  template <Automaton Aut>
  states_t<Aut>
  coaccessible_states(const Aut& a, bool strict = true)
  {
    return accessible_states(transpose(a), strict);
  }

  /// The set of useful states, including possibly pre() and post().
  ///
  /// \param a       the automaton.
  /// \param strict  whether to evaluate lazy states.
  template <Automaton Aut>
  states_t<Aut>
  useful_states(const Aut& a, bool strict = true)
  {
    auto accessible = accessible_states(a, strict);
    auto coaccessible = coaccessible_states(a, strict);
    return set_intersection(accessible, coaccessible);
  }


  /*----------------------------------------------------.
  | Number of accessible, coaccessible, useful states.  |
  `----------------------------------------------------*/

  /// Number of accessible states, not counting pre() and post().
  template <Automaton Aut>
  size_t
  num_accessible_states(const Aut& a)
  {
    auto set = accessible_states(a);
    size_t res = set.size();
    // Don't count pre().
    res -= 1;
    // Don't count post().
    if (has(set, a->post()))
      res -= 1;
    return res;
  }

  /// Number of accessible states, not counting pre() and post().
  template <Automaton Aut>
  size_t
  num_coaccessible_states(const Aut& a)
  {
    return num_accessible_states(transpose(a));
  }

  /// Number of accessible states, not counting pre() and post().
  template <Automaton Aut>
  size_t
  num_useful_states(const Aut& a)
  {
    auto set = useful_states(a);
    size_t res = set.size();
    // Don't count pre().
    if (has(set, a->pre()))
      res -= 1;
    // Don't count post().
    if (has(set, a->post()))
      res -= 1;
    return res;
  }


  /*-----------------------------------------------.
  | accessible, coaccessible, useful subautomata.  |
  `-----------------------------------------------*/

  /// Accessible part of an automaton.
  template <Automaton Aut>
  filter_automaton<Aut>
  accessible(const Aut& a)
  {
    auto res = vcsn::filter(a, accessible_states(a));
    res->properties().update(accessible_ftag{});
    return res;
  }

  /// Coaccessible part of an automaton.
  template <Automaton Aut>
  filter_automaton<Aut>
  coaccessible(const Aut& a)
  {
    auto res = vcsn::filter(a, coaccessible_states(a));
    res->properties().update(coaccessible_ftag{});
    return res;
  }

  /// Useful part of an automaton.
  template <Automaton Aut>
  filter_automaton<Aut>
  trim(const Aut& a)
  {
    auto res = vcsn::filter(a, useful_states(a));
    res->properties().update(trim_ftag{});
    return res;
  }

  /*----------------------------------------------------------------.
  | is_trim, is_accessible, is_coaccessible, is_empty, is_useless.  |
  `----------------------------------------------------------------*/

  /// Whether all its states are useful.
  template <Automaton Aut>
  bool is_trim(const Aut& a)
  {
    return num_useful_states(a) == a->num_states();
  }

  /// Whether all no state is useful.
  template <Automaton Aut>
  bool is_useless(const Aut& a)
  {
    return num_useful_states(a) == 0;
  }

  /// Whether all its states are accessible.
  template <Automaton Aut>
  bool is_accessible(const Aut& a)
  {
    return num_accessible_states(a) == a->num_states();
  }

  /// Whether all its states are coaccessible.
  template <Automaton Aut>
  bool is_coaccessible(const Aut& a)
  {
    return num_coaccessible_states(a) == a->num_states();
  }

  template <Automaton Aut>
  bool is_empty(const Aut& a) ATTRIBUTE_PURE;

  /// Whether has no states.
  template <Automaton Aut>
  bool is_empty(const Aut& a)
  {
    // FIXME: Beware of the case where there is a transition from
    // pre() to post().
    return a->num_states() == 0;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      accessible(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::accessible(a);
      }

      /// Bridge.
      template <Automaton Aut>
      automaton
      coaccessible(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::coaccessible(a);
      }

      /// Bridge.
      template <Automaton Aut>
      automaton
      trim(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::trim(a);
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_accessible(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_accessible(a);
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_coaccessible(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_coaccessible(a);
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_trim(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_trim(a);
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_useless(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_useless(a);
      }

      /// Bridge.
      template <Automaton Aut>
      bool
      is_empty(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_empty(a);
      }
    }
  }
}
