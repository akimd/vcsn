#pragma once

#include <vcsn/misc/memory.hh> // make_shared_ptr.

namespace vcsn
{

  // vcsn/misc/crange.hh
  template <typename C>
  struct container_range;

  template <typename Cont, typename Pred>
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

  // vcsn/core/name-automaton.hh
  namespace detail
  {
    template <typename Context>
    class name_automaton_impl;
  }
  template <typename Context>
  using name_automaton
    = std::shared_ptr<detail::name_automaton_impl<Context>>;


  // vcsn/core/permutation-automaton.hh.
  namespace detail
  {
    template <typename Aut>
    class permutation_automaton_impl;
  }

  /// A permutation automaton as a shared pointer.
  template <typename Aut>
  using permutation_automaton
    = std::shared_ptr<detail::permutation_automaton_impl<Aut>>;


  // vcsn/core/expression-automaton.hh.
  namespace detail
  {
    template <typename Aut>
    class expression_automaton_impl;
  }

  /// An expression automaton as a shared pointer.
  template <typename Aut>
  using expression_automaton
    = std::shared_ptr<detail::expression_automaton_impl<Aut>>;

} // namespace vcsn
