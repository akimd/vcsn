#pragma once

#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/transpose.hh>

namespace vcsn
{

  /*-------------------------------------------.
  | minimization with Brzozowski's algorithm.  |
  `-------------------------------------------*/

  template <typename Aut>
  inline
  auto
  minimize_brzozowski(const Aut& a)
    -> decltype(determinize(codeterminize(a)))
  {
    return determinize(codeterminize(a));
  }

  template <typename Aut>
  inline
  auto
  cominimize_brzozowski(const Aut& a)
    -> decltype(transpose(minimize_brzozowski(transpose(a))))
  {
    return transpose(minimize_brzozowski(transpose(a)));
  }

} // namespace vcsn
