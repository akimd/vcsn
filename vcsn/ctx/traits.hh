#ifndef VCSN_CTX_TRAITS_HH
# define VCSN_CTX_TRAITS_HH

# include <memory>

namespace vcsn
{

  /*---------------.
  | context_t_of.  |
  `---------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct context_t_of_impl
    {
      using type = typename ValueSet::context_t;
    };
  }

  template <typename ValueSet>
  using context_t_of = typename detail::context_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  /*-------------.
  | label_t_of.  |
  `-------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct label_t_of_impl
    {
      using type = typename ValueSet::label_t;
    };
  }

  template <typename ValueSet>
  using label_t_of = typename detail::label_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  /*----------------.
  | labelset_t_of.  |
  `----------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct labelset_t_of_impl
    {
      using type = typename ValueSet::labelset_t;
    };
  }

  template <typename ValueSet>
  using labelset_t_of = typename detail::labelset_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  /*-------------.
  | state_t_of.  |
  `-------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct state_t_of_impl
    {
      using type = typename ValueSet::state_t;
    };
  }

  template <typename ValueSet>
  using state_t_of = typename detail::state_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  /*------------------.
  | transition_t_of.  |
  `------------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct transition_t_of_impl
    {
      using type = typename ValueSet::transition_t;
    };
  }

  template <typename ValueSet>
  using transition_t_of = typename detail::transition_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  /*--------------.
  | weight_t_of.  |
  `--------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct weight_t_of_impl
    {
      using type = typename ValueSet::weight_t;
    };
  }

  template <typename ValueSet>
  using weight_t_of = typename detail::weight_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  /*-----------------.
  | weightset_t_of.  |
  `-----------------*/
  namespace detail
  {
    template <typename ValueSet>
    struct weightset_t_of_impl
    {
      using type = typename ValueSet::weightset_t;
    };
  }

  template <typename ValueSet>
  using weightset_t_of = typename detail::weightset_t_of_impl<typename std::remove_cv<ValueSet>::type>::type;


  namespace detail
  {
#define DEFINE(Traits)                                                  \
    template <typename ValueSet>                                        \
    struct Traits ## _t_of_impl<std::shared_ptr<ValueSet>>              \
      : Traits ## _t_of_impl<typename std::remove_cv<ValueSet>::type>   \
    {}

    DEFINE(context);
    DEFINE(label);
    DEFINE(labelset);
    DEFINE(state);
    DEFINE(transition);
    DEFINE(weight);
    DEFINE(weightset);
#undef DEFINE
  }
}

#endif // !VCSN_CTX_TRAITS_HH
