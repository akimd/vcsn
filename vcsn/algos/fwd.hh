#ifndef VCSN_ALGOS_FWD_HH
# define VCSN_ALGOS_FWD_HH

namespace vcsn
{
  template <typename Aut>
  bool in_situ_remover(Aut& aut);

  template <typename Aut>
  bool is_proper(const Aut& aut);

  template <typename Aut>
  bool is_valid(const Aut& aut);

}

#endif // !VCSN_ALGOS_FWD_HH

