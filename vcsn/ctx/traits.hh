#pragma once

#include <memory>

#include <vcsn/labelset/fwd.hh> // tupleset.
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{

  /// T without reference or const/volatile qualifiers.
  template <typename T>
  using base_t = std::remove_cv_t<std::remove_reference_t<T>>;

  /*------------------.
  | Computing types.  |
  `------------------*/

  /// Extract various ValueSets/Value types from objects, or pointers
  /// to objects.
  ///
  /// Example of valid uses:
  ///
  /// labelset_t_of<mutable_automaton_impl> // a class
  /// labelset_t_of<mutable_automaton>      // a shared_ptr
  ///
  /// SFINAE compliant.

#define DEFINE(Type)                                                    \
  namespace detail                                                      \
  {                                                                     \
    template <typename ValueSet, typename = void_t<>>                   \
    struct Type ## _of_impl;                                            \
                                                                        \
    template <typename ValueSet>                                        \
    struct Type ## _of_impl<ValueSet,                                   \
                            void_t<typename ValueSet::Type>>            \
    {                                                                   \
      using type = typename ValueSet::Type;                             \
    };                                                                  \
                                                                        \
    template <typename ValueSet>                                        \
    struct Type ## _of_impl<std::shared_ptr<ValueSet>,                  \
                            void_t<typename ValueSet::Type>>            \
    {                                                                   \
      using type = typename ValueSet::Type;                             \
    };                                                                  \
                                                                        \
    template <typename ValueSet>                                        \
    struct Type ## _of_impl<ValueSet*,                                  \
                            void_t<typename ValueSet::Type>>            \
    {                                                                   \
      using type = typename ValueSet::Type;                             \
    };                                                                  \
  }                                                                     \
                                                                        \
  template <typename ValueSet>                                          \
  using Type ## _of                                                     \
    = typename detail::Type ## _of_impl<base_t<ValueSet>>::type

  DEFINE(context_t);
  DEFINE(label_t);
  DEFINE(labelset_t);
  DEFINE(state_t);
  DEFINE(transition_t);
  DEFINE(weight_t);
  DEFINE(weightset_t);

#undef DEFINE

  /// Given an automaton type, the type of its copies.
  ///
  /// Strips most decorators, with the notable exception of
  /// transpose_automaton: copies of transposed automata are
  /// transposed automata, but with all other kinds of decorations
  /// removed.  This allows a simple implementation of
  /// "co-algorithms": transpose the input automaton, create a fresh
  /// copy (which is therefore transposed), process it, and then
  /// return its transposition, i.e., a forward automaton.
  template <typename Aut, typename Context = context_t_of<Aut>>
  using fresh_automaton_t_of
    = typename Aut::element_type::template fresh_automaton_t<Context>;

  template <typename ValueSet>
  using letter_t_of
    = typename labelset_t_of<base_t<ValueSet>>::letter_t;

  template <typename ValueSet>
  using word_t_of
    = typename labelset_t_of<base_t<ValueSet>>::word_t;

  /// Whether two labelsets are composable.
  template <typename LS1, typename LS2>
  struct are_labelsets_composable : std::false_type
  {};

  template <typename LS1, typename LS2, typename LS3>
  struct are_labelsets_composable<tupleset<LS1, LS2>, tupleset<LS2, LS3>>
    : std::true_type
  {};

  /// Whether two contexts are composable.
  template <typename Ctx1, typename Ctx2>
  struct are_composable
    : are_labelsets_composable<labelset_t_of<Ctx1>, labelset_t_of<Ctx2>>
  {};
}
