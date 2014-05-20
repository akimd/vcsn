#ifndef VCSN_CORE_FWD_HH
# define VCSN_CORE_FWD_HH

# include <memory>

namespace vcsn
{

  // vcsn/core/crange.hh
  template <class C>
  struct container_range;

  template <class C>
  struct container_filter_range;

  // vcsn/core/mutable_automaton.hh
  namespace detail
  {
    template <typename Context>
    class mutable_automaton_impl;
  }
  template <typename Context>
  using mutable_automaton
    = std::shared_ptr<detail::mutable_automaton_impl<Context>>;

} // namespace vcsn

#endif // !VCSN_CORE_FWD_HH
