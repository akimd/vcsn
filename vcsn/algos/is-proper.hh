#pragma once

#include <type_traits>

#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/dyn/automaton.hh> // make_automaton

namespace vcsn
{

  namespace detail
  {
    template <Automaton Aut>
    inline
    std::enable_if_t<labelset_t_of<Aut>::has_one(), bool>
    is_proper_(const Aut& aut)
    {
      for (auto t: transitions(aut))
        if (aut->labelset()->is_one(aut->label_of(t)))
          return false;
      return true;
    }

    template <Automaton Aut>
    inline constexpr
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
  inline
  bool
  is_proper(const Aut& aut)
  {
    return detail::is_proper_(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      inline
      bool is_proper(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_proper(a);
      }
    }
  }
} // namespace vcsn
