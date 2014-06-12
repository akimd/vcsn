#ifndef VCSN_ALGOS_FWD_HH
# define VCSN_ALGOS_FWD_HH

# include <memory>

namespace vcsn
{
  // Beware that this file is dangerous, as it prevents automatic
  // instantiation of these algos.  It exists only to solve mutual
  // dependencies (e.g.: proper uses is-valid which uses proper).
  //
  // FIXME: We should probably resolves these issues in some other
  // way.

  template <typename Aut>
  bool in_situ_remover(Aut& aut, bool prune = true);

  template <typename Aut>
  bool is_proper(const Aut& aut);

  template <typename Aut>
  bool is_valid(const Aut& aut);

  namespace detail_info
  {
    template <typename Aut>
    size_t
    num_eps_transitions(const Aut&);
  }

  namespace detail
  {
    template <std::size_t Band, typename Aut>
    class blind_automaton_impl;

    template <std::size_t Band, typename Aut>
    using blind_automaton
      = std::shared_ptr<blind_automaton_impl<Band, Aut>>;
  }

  // vcsn/algos/transpose.hh
  namespace detail
  {
    template <typename Aut>
    class transpose_automaton_impl;
  }

  template <typename Aut>
  using transpose_automaton
    = std::shared_ptr<detail::transpose_automaton_impl<Aut>>;
}

#endif // !VCSN_ALGOS_FWD_HH
