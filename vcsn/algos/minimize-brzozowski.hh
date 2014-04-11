#ifndef VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH
# define VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH

# include <vcsn/algos/copy.hh>
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
    // FIXME: this is ugly, we want a "transposed" routine that
    // returns a copy.
    auto aut = vcsn::copy(a);
    auto codet = determinize(transpose(aut));
    // FIXME: still ugly: transpose produces a transpose_automaton,
    // which is the type determinize will use.  To get back a
    // mutable_automaton, don't tranpose the result again.
    //
    // FIXME: transpose(transpose(a)) should be the identity.
    return determinize(*codet.original_automaton());
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH
