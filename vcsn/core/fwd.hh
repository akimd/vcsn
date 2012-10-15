#ifndef VCSN_CORE_FWD_HH
# define VCSN_CORE_FWD_HH

# include <memory> // shared_ptr

namespace vcsn
{

  // crange.hh
  template <class C>
  struct container_range;

  template <class C>
  struct container_filter_range;

  // automaton.hh
  namespace dyn
  {
    class abstract_automaton;
    using automaton = std::shared_ptr<abstract_automaton>;
  }

  // mutable_automaton.hh
  template <typename Context>
  class mutable_automaton;

} // namespace vcsn

#endif // !VCSN_CORE_FWD_HH
