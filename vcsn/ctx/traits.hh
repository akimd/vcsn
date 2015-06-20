#pragma once

#include <memory>

#include <vcsn/misc/type_traits.hh>

namespace vcsn
{

  /// T without reference or const/volatile qualifiers.
  template <typename T>
  using base_t = remove_cv_t<remove_reference_t<T>>;

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

  template <typename Aut, typename Context = context_t_of<Aut>>
  using fresh_automaton_t_of
    = typename Aut::element_type::template fresh_automaton_t<Context>;

  template <typename ValueSet>
  using letter_t_of
    = typename labelset_t_of<base_t<ValueSet>>::letter_t;

  template <typename ValueSet>
  using word_t_of
    = typename labelset_t_of<base_t<ValueSet>>::word_t;
}
