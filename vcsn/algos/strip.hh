#pragma once

#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace detail
  {
    // Rely on the fact that int takes precedence over long to express
    // a precedence of this first function over the second one.

    // automata that feature a strip member function.
    template <Automaton Aut>
    auto
    strip(const Aut& aut, int)
      -> decltype(aut->strip())
    {
      return aut->strip();
    }

    // automata that don't feature a strip member function.
    template <Automaton Aut>
    auto
    strip(const Aut& aut, long)
      -> decltype(aut)
    {
      return aut;
    }
  }

  /// Remove (all) the decorations from a decorated automaton.
  template <Automaton Aut>
  auto
  strip(const Aut& aut)
  {
    return detail::strip(aut, 0);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      strip(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::strip(a);
      }
    }
  }
} // vcsn::
