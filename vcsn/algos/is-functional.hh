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
    auto l = focus<0>(aut);
    auto r = insplit(l);
    auto compose = detail::make_composer(l, r);
    auto c = compose();
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
