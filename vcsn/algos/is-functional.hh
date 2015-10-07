#pragma once

#include <vcsn/algos/compose.hh>
#include <vcsn/algos/is-partial-identity.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  /*---------------.
  | is-functional. |
  `---------------*/

  /// Whether \a aut is functional.
  /// \pre \a aut is a transducer.
  template <Automaton Aut>
  bool is_functional(const Aut& aut)
  {
    // Compose aut and its invert.
    auto c = make_compose_automaton<Aut, Aut, 0, 0>(aut, aut);
    c->compose();
    return is_partial_identity(c);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_functional(const automaton& aut)
      {
        return is_functional(aut->as<Aut>());
      }
    }
  }
}
