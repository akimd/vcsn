#pragma once

#include <vcsn/core/mutable-automaton.hh>

namespace vcsn
{
  namespace detail
  {

    /// An automaton whose type is the join between those of \a auts.
    template <Automaton... Auts>
    auto
    join_automata(Auts&&... auts)
      // SFINAE
      -> decltype(pass(auts->null_state()...),
                  make_mutable_automaton(join(auts->context()...)))
    {
      return make_mutable_automaton(join(auts->context()...));
    }

    /// The type of the join between automata of type Auts.
    template <Automaton... Auts>
    using join_automata_t = decltype(join_automata<std::declval<Auts>()...>);


    /// The nullable context of the join between parameters' context.
    template <Automaton... Auts>
    auto
    nullable_join_context(Auts&&... auts)
      -> decltype(pass(auts->null_state()...),
                  make_nullableset_context(join(auts->context()...)))
    {
      return make_nullableset_context(join(auts->context()...));
    }

    /// An automaton whose type is the nullable join between those of \a auts.
    template <Automaton... Auts>
    auto
    nullable_join_automata(Auts&&... auts)
      -> decltype(pass(auts->null_state()...),
                  make_mutable_automaton(nullable_join_context(auts...)))
    {
      return make_mutable_automaton(nullable_join_context(auts...));
    }

    /// An automaton whose type is the meet between those of \a auts.
    template <Automaton... Auts>
    auto
    meet_automata(Auts&&... auts)
      -> decltype(pass(auts->null_state()...),
                  make_mutable_automaton(meet(auts->context()...)))
    {
      return make_mutable_automaton(meet(auts->context()...));
    }
  }
}
