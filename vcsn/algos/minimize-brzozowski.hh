#pragma once

#include <vcsn/misc/attributes.hh>
#include <vcsn/algos/is-free-boolean.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/transpose.hh>

namespace vcsn
{

  /*-------------------------------------------.
  | minimization with Brzozowski's algorithm.  |
  `-------------------------------------------*/

  /// Request for Brzozowski implementation of minimize (B and free).
  struct brzozowski_tag {};

  /// The type of the codeterminized automaton.
  /// Use decltype on transpose because transpose(transpose(a)) returns a.
  template <Automaton Aut>
  using codeterminized_automaton
    = transpose_automaton<determinized_automaton
                          <decltype(transpose(std::declval<Aut>())),
                           wet_kind_t::bitset>>;

  /// Brzozowski-based minimization.
  template <Automaton Aut>
  auto
  minimize(const Aut& a, brzozowski_tag)
    -> std::enable_if_t<is_free_boolean<Aut>(),
                        determinized_automaton<codeterminized_automaton<Aut>,
                                               wet_kind_t::bitset>>
  {
    return determinize(codeterminize(a));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Handling of errors for dyn::minimize.
      template <Automaton Aut>
      ATTRIBUTE_NORETURN
      std::enable_if_t<!is_free_boolean<Aut>(), Aut>
      minimize(const Aut&, brzozowski_tag)
      {
        raise("minimize: invalid algorithm"
              " (non-Boolean or non-free labelset):",
              " brzozowski");
      }
    }
  }
} // namespace vcsn
