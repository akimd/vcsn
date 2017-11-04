#pragma once

#include <vcsn/algos/is-proper.hh>
#include <vcsn/concepts/automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  /*----------.
  | is_free.  |
  `----------*/

  namespace detail
  {
    template <Automaton Aut>
    bool is_free_(const Aut& aut)
    {
      return aut->labelset()->is_letterized()
        && is_proper_tapes_(aut);
    }
  }

  /// Whether an automaton is free.
  ///
  /// An automaton is free iff it is letterized and proper.
  ///
  /// \param aut The tested automaton
  /// \return Whether the automaton is free
  template <Automaton Aut>
  bool is_free(const Aut& aut)
  {
    if (aut->properties().is_unknown(*aut, is_free_ptag{}))
      {
        auto res = is_free_(aut);
        aut->properties().put(*aut, is_free_ptag{}, res);
        return res;
      }
    else
      return aut->properties().get(*aut, is_free_ptag{});
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_free(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_free(a);
      }
    }
  }

  /*------------------------.
  | is_letterized_boolean.  |
  `------------------------*/

  /// Whether the automaton is free, and the weightset is B.
  /// I.e., a PODFA (plain old dfa).
  template <Automaton Aut>
  constexpr bool is_letterized_boolean()
  {
    return labelset_t_of<Aut>::is_letterized()
      && std::is_same<vcsn::weightset_t_of<Aut>, vcsn::b>::value;
  }
}
