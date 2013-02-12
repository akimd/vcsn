#ifndef VCSN_EMPTY_HH
# define VCSN_EMPTY_HH

namespace vcsn
{
  /// Empty labels, for LAU.
  struct empty_t
  {};

  // So that we can make a map with empty_t as key, for
  // mutable_automaton's entries.
  inline
  bool operator<(empty_t, empty_t)
  {
    return false;
  }
}

#endif // !VCSN_EMPTY_HH
