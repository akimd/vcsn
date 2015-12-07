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

  template <typename Aut>
  using codeterminized_automaton
    = transpose_automaton<determinized_automaton<Aut>>;

  template <typename Aut>
  auto
  minimize(const Aut& a, brzozowski_tag)
    -> vcsn::enable_if_t<is_free_boolean<Aut>(),
                         determinized_automaton<codeterminized_automaton<decltype(transpose(a))>>>
  {
    return determinize(codeterminize(a));
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      ATTRIBUTE_NORETURN
      vcsn::enable_if_t<!is_free_boolean<Aut>(), Aut>
      minimize(const Aut&, brzozowski_tag)
      {
        raise("minimize: invalid algorithm (non-Boolean or non-free labelset): ",
              "brzozowski");
      }
    }
  }
} // namespace vcsn
