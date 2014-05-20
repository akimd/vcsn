#ifndef VCSN_CTX_TRAITS_HH
# define VCSN_CTX_TRAITS_HH

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
  using context_t_of = typename detail::context_t_of_impl<ValueSet>::type;


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
  using label_t_of = typename detail::label_t_of_impl<ValueSet>::type;


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
  using labelset_t_of = typename detail::labelset_t_of_impl<ValueSet>::type;


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
  using state_t_of = typename detail::state_t_of_impl<ValueSet>::type;


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
  using transition_t_of = typename detail::transition_t_of_impl<ValueSet>::type;


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
  using weight_t_of = typename detail::weight_t_of_impl<ValueSet>::type;


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
  using weightset_t_of = typename detail::weightset_t_of_impl<ValueSet>::type;
}

#endif // !VCSN_CTX_TRAITS_HH
