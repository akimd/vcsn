#ifndef VCSN_CORE_FWD_HH
# define VCSN_CORE_FWD_HH

# include <memory> // shared_ptr

namespace vcsn
{

  // vcsn/core/crange.hh
  template <class C>
  struct container_range;

  template <class C>
  struct container_filter_range;

  // vcsn/core/mutable_automaton.hh
  template <typename Context>
  class mutable_automaton;

} // namespace vcsn

#endif // !VCSN_CORE_FWD_HH
