#pragma once

#include <vcsn/algos/standard.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(normalize);

  /*------------.
  | Automata.   |
  `------------*/

  /// Whether \a a is standard and co-standard.
  template <Automaton Aut>
  bool
  is_normalized(const Aut& a)
  {
    return is_standard(a) && is_costandard(a);
  }

  /// Normalize automaton \a a.
  template <Automaton Aut>
  auto
  normalize(const Aut& a)
    -> decltype(copy(a))
  {
    auto res = costandard(standard(a));
    res->properties().update(normalize_ftag{});
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool
      is_normalized(const automaton& aut)
      {
        return is_normalized(aut->as<Aut>());
      }

      /// Bridge.
      template <Automaton Aut>
      automaton
      normalize(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::normalize(a);
      }
    }
  }
} // vcsn::
