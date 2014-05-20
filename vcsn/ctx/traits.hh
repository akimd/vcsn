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
}

#endif // !VCSN_CTX_TRAITS_HH
