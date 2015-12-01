#pragma once

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
    -> determinized_automaton<codeterminized_automaton<decltype(transpose(a))>>
  {
    return determinize(codeterminize(a));
  }

  template <typename Aut>
  auto
  cominimize(const Aut& a, brzozowski_tag)
    -> decltype(transpose(minimize(transpose(a), brzozowski_tag{})))
  {
    return transpose(minimize(transpose(a), brzozowski_tag{}));
  }

} // namespace vcsn
