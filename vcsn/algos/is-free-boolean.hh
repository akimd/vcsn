#pragma once

#include <vcsn/concepts/automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  template <Automaton Aut>
  constexpr bool is_free_boolean()
  {
    return vcsn::labelset_t_of<Aut>::is_free()
      && std::is_same<vcsn::weightset_t_of<Aut>, vcsn::b>::value;
  }
}
