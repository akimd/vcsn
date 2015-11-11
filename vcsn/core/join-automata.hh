#pragma once

#include <vcsn/core/mutable-automaton.hh>

namespace vcsn
{
  namespace detail
  {

    /// An automaton whose type is the join between those of \a auts.
    template <typename... Auts>
    auto
    join_automata(Auts&&... auts)
      -> decltype(make_mutable_automaton(join(auts->context()...)))
    {
      return make_mutable_automaton(join(auts->context()...));
    }

    /// The type of the join between automata of type Auts.
    template <typename... Auts>
    using join_automata_t = decltype(join_automata<std::declval<Auts>()...>);


    /// The nullable context of the join between parameters' context.
    template <typename... Auts>
    auto
    nullable_join_context(Auts&&... auts)
      -> decltype(make_nullableset_context(join(auts->context()...)))
    {
      return make_nullableset_context(join(auts->context()...));
    }

    /// An automaton whose type is the nullable join between those of \a auts.
    template <typename... Auts>
    auto
    nullable_join_automata(Auts&&... auts)
      -> decltype(make_mutable_automaton(nullable_join_context(auts...)))
    {
      return make_mutable_automaton(nullable_join_context(auts...));
    }

    /// An automaton whose type is the meet between those of \a auts.
    template <typename... Auts>
    auto
    meet_automata(Auts&&... auts)
      -> decltype(make_mutable_automaton(meet(auts->context()...)))
    {
      return make_mutable_automaton(meet(auts->context()...));
    }

  }
}
