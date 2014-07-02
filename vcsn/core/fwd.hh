#ifndef VCSN_CORE_FWD_HH
# define VCSN_CORE_FWD_HH

# include <vcsn/misc/memory.hh> // make_shared_ptr.

namespace vcsn
{

  // vcsn/core/crange.hh
  template <class C>
  struct container_range;

  template <class C>
  struct container_filter_range;


  // vcsn/core/mutable-automaton.hh
  namespace detail
  {
    template <typename Context>
    class mutable_automaton_impl;
  }
  template <typename Context>
  using mutable_automaton
    = std::shared_ptr<detail::mutable_automaton_impl<Context>>;


  // vcsn/core/ratexp-automaton.hh.
  namespace detail
  {
    template <typename Aut>
    class ratexp_automaton_impl;
  }
  template <typename Aut>
  using ratexp_automaton
    = std::shared_ptr<detail::ratexp_automaton_impl<Aut>>;

} // namespace vcsn

#endif // !VCSN_CORE_FWD_HH
