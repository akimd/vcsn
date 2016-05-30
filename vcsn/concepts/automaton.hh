#pragma once

#include <vcsn/ctx/traits.hh> // state_t_of

namespace vcsn
{
#if defined __cpp_concepts
  template <typename Aut>
  concept bool Automaton()
  {
    return requires (Aut a)
      {
        typename state_t_of<Aut>;
        { a->null_state() } -> state_t_of<Aut>;
        { a->pre() } -> state_t_of<Aut>;
        { a->post() } -> state_t_of<Aut>;

        typename transition_t_of<Aut>;
        { a->null_transition() } -> transition_t_of<Aut>;
      };
  }
#else
# define Automaton typename
#endif
}
