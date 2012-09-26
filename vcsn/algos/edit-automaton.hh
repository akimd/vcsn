#ifndef VCSN_ALGOS_EDIT_AUTOMATON_HH
# define VCSN_ALGOS_EDIT_AUTOMATON_HH

namespace vcsn
{
  /// Add transitions from src to dst, labeled by \c entry.
  template <typename Aut>
  void
  add_entry(Aut& a,
            typename Aut::state_t src, typename Aut::state_t dst,
            const std::string& entry)
  {
    assert (src != a.pre());
    assert (dst != a.post());
    a.add_entry(src, dst, a.entryset().conv(entry));
  }

  /// Add transitions from src to dst, possibly being pre() xor post().
  template <typename Aut>
  void
  add_entry(Aut& a,
            typename Aut::state_t src, typename Aut::state_t dst,
            const std::string* entry)
  {
    if (entry)
      add_entry(a, src, dst, *entry);
    else
      {
        assert (src == a.pre() || dst == a.post());
        a.add_transition(src, dst, a.prepost_label());
      }
  }

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
