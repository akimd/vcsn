#pragma once

#include <type_traits>

#include <vcsn/core/automaton.hh> // transitions.
#include <vcsn/core/kind.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // make_automaton
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/attributes.hh>

namespace vcsn
{

  namespace detail
  {
    /// Whether a one appears on at least one tape of any transition.
    template <Automaton Aut>
    std::enable_if_t<is_multitape<labelset_t_of<Aut>>{}, bool>
    is_proper_tapes_(const Aut& aut)
    {
      for (auto t: transitions(aut))
        if (aut->labelset()->show_one(aut->label_of(t)))
          return false;
      return true;
    }

    template <Automaton Aut>
    std::enable_if_t<!is_multitape<labelset_t_of<Aut>>{}, bool>
    is_proper_tapes_(const Aut& aut)
    {
      return is_proper(aut);
    }

    template <Automaton Aut>
    std::enable_if_t<labelset_t_of<Aut>::has_one(), bool>
    is_proper_(const Aut& aut)
    {
      for (auto t: transitions(aut))
        if (aut->labelset()->is_one(aut->label_of(t)))
          return false;
      return true;
    }

    template <Automaton Aut>
    constexpr
    std::enable_if_t<!labelset_t_of<Aut>::has_one(), bool>
    is_proper_(const Aut&)
    {
      return true;
    }
  }

  /**@brief Test whether an automaton is proper.

     An automaton is proper iff it contains no epsilon-transition.

     @param aut The tested automaton
     @return true iff the automaton is proper
  */
  template <Automaton Aut>
  bool is_proper(const Aut& aut) ATTRIBUTE_CONST;

  template <Automaton Aut>
  bool
  is_proper(const Aut& aut)
  {
    if (aut->properties().is_unknown(*aut, is_proper_ptag{}))
    {
      auto res = detail::is_proper_(aut);
      aut->properties().put(*aut, is_proper_ptag{}, res);
      return res;
    }
    else
      return aut->properties().get(*aut, is_proper_ptag{});
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_proper(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_proper(a);
      }
    }
  }
} // namespace vcsn
