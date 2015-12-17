#pragma once

#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/algos/bellman-ford.hh>
#include <vcsn/weightset/fwd.hh>

namespace vcsn
{

  /*---------------------.
  | has_negative_cycle.  |
  `---------------------*/

  template <Automaton Aut>
  std::enable_if_t<weightset_t_of<Aut>::can_have_negative_cycle(), bool>
  has_negative_cycle(const Aut& aut)
  {
    return !detail::bellman_ford_impl(aut, aut->pre());
  }

  template <Automaton Aut>
  std::enable_if_t<!weightset_t_of<Aut>::can_have_negative_cycle(), bool>
  has_negative_cycle(const Aut& aut)
  {
    return false;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge
      template <Automaton Aut>
      bool has_negative_cycle(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::has_negative_cycle(a);
      }
    }
  }
}
