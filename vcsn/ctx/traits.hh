#ifndef VCSN_CTX_TRAITS_HH
# define VCSN_CTX_TRAITS_HH

# include <memory>

namespace vcsn
{

  /// T without reference or const/volatile qualifiers.
  template <typename T>
  using base_t
    = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

  /*------------------.
  | Computing types.  |
  `------------------*/

# define DEFINE(Type)                                                   \
  namespace detail                                                      \
  {                                                                     \
    template <typename ValueSet>                                        \
    struct Type ## _of_impl                                             \
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

# undef DEFINE


  /*------------------.
  | shared pointers.  |
  `------------------*/

  // Forward traits about shared_ptr<T> to T.
  namespace detail
  {
# define DEFINE(Traits)                                         \
    template <typename ValueSet>                                \
    struct Traits ## _of_impl<std::shared_ptr<ValueSet>>        \
      : Traits ## _of_impl<base_t<ValueSet>>                    \
    {}

    DEFINE(context_t);
    DEFINE(label_t);
    DEFINE(labelset_t);
    DEFINE(state_t);
    DEFINE(transition_t);
    DEFINE(weight_t);
    DEFINE(weightset_t);
# undef DEFINE
  }
}

#endif // !VCSN_CTX_TRAITS_HH
