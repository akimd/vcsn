#pragma once

#include <memory>
#include <type_traits>

namespace vcsn
{

  /// T without reference or const/volatile qualifiers.
  template <typename T>
  using base_t
    = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

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

#define DEFINE(Type)                                                    \
  namespace detail                                                      \
  {                                                                     \
    template <typename ValueSet>                                        \
    struct Type ## _of_impl                                             \
    {                                                                   \
      using type = typename ValueSet::Type;                             \
    };                                                                  \
                                                                        \
    template <typename ValueSet>                                        \
    struct Type ## _of_impl<std::shared_ptr<ValueSet>>                  \
      : Type ## _of_impl<base_t<ValueSet>>                              \
    {};                                                                 \
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

  template <typename ValueSet>
  using letter_t_of
    = typename labelset_t_of<base_t<ValueSet>>::letter_t;

  template <typename ValueSet>
  using word_t_of
    = typename labelset_t_of<base_t<ValueSet>>::word_t;
}
