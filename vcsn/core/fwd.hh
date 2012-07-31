#ifndef VCSN_CORE_FWD_HH
# define VCSN_CORE_FWD_HH

namespace vcsn
{

  // crange.hh
  template <class C>
  struct container_range;

  template <class C>
  struct container_filter_range;

  // mutable_automaton.hh
  class abstract_mutable_automaton;

  template <typename Context>
  class mutable_automaton;

} // namespace vcsn

#endif // !VCSN_CORE_FWD_HH
