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
    -> vcsn::enable_if_t<labelset_t_of<Aut>::is_free()
                         && std::is_same<weightset_t_of<Aut>, b>::value,
                         determinized_automaton<codeterminized_automaton<decltype(transpose(a))>>>
  {
    return determinize(codeterminize(a));
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      auto
      minimize(const Aut&, brzozowski_tag)
        -> vcsn::enable_if_t<!labelset_t_of<Aut>::is_free()
                             || !std::is_same<weightset_t_of<Aut>, b>::value,
                             Aut>
      {
        raise("minimize: invalid algorithm (non-Boolean or non-free labelset): ",
              "brzozowski");
      }
    }
  }
} // namespace vcsn
