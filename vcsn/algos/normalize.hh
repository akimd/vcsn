#pragma once

#include <vcsn/algos/standard.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /// Whether \a a is standard and co-standard.
  template <Automaton Aut>
  bool
  is_normalized(const Aut& a)
  {
    return is_standard(a) && is_costandard(a);
  }

  /// Normalize \a a automaton.
  template <Automaton Aut>
  auto
  normalize(const Aut& a)
    -> decltype(copy(a))
  {
    return costandard(standard(a));
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
