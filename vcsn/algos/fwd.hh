#ifndef VCSN_ALGOS_FWD_HH
# define VCSN_ALGOS_FWD_HH

# include <memory>

// Beware that this file is dangerous, as it prevents automatic
// instantiation of these algos.  It exists only to solve mutual
// dependencies (e.g.: proper uses is-valid which uses proper).
//
// FIXME: We should probably resolves these issues in some other way.

namespace vcsn
{
  // vcsn/algos/blind.hh
  namespace detail
  {
    template <std::size_t TapeNum, typename Aut>
    class blind_automaton_impl;
  }
  template <std::size_t TapeNum, typename Aut>
  using blind_automaton
    = std::shared_ptr<detail::blind_automaton_impl<TapeNum, Aut>>;

  // vcsn/algos/edit-automaton.hh.
  class automaton_editor;

  template <typename Aut>
  bool in_situ_remover(Aut& aut, bool prune = true);

  template <typename Aut>
  bool is_ambiguous(const Aut& aut);

  template <typename Aut>
  bool is_proper(const Aut& aut);

  template <typename Aut>
  bool is_valid(const Aut& aut);

  template <typename RatExpSet>
  bool is_valid(const RatExpSet&, const typename RatExpSet::ratexp_t&);

  namespace detail_info
  {
    template <typename Aut>
    size_t
    num_eps_transitions(const Aut&);
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
