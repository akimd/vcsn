#ifndef VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH
# define VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH

# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/strip.hh>
# include <vcsn/algos/transpose.hh>

namespace vcsn
{

  /*-------------------------------------------.
  | minimization with Brzozowski's algorithm.  |
  `-------------------------------------------*/

  template <typename Aut>
  inline
  typename Aut::element_type::automaton_nocv_t
  minimize_brzozoski(const Aut& a)
  {
    auto codet = strip(determinize(transpose(strip(a))));
    return strip(determinize(transpose(codet)));
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_BRZOZOWSKI_HH
