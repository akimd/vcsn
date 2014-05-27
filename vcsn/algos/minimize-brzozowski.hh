#ifndef VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH
# define VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH

# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/transpose.hh>

namespace vcsn
{

  /*-------------------------------------------.
  | minimization with Brzozowski's algorithm.  |
  `-------------------------------------------*/

  template <typename Aut>
  inline
  Aut
  minimize_brzozoski(const Aut& a)
  {
    auto codet = determinize(transpose(a))->original_automaton();
    return determinize(transpose(codet))->original_automaton();
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH
