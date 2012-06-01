#ifndef VCSN_MISC_CONST_TRAITS_HH
# define VCSN_MISC_CONST_TRAITS_HH

namespace vcsn
{

  namespace misc
  {

    template <typename T>
    struct constify_traits
    {
      using type = const T;
    };

    template <typename T>
    struct id_traits
    {
      using type = T;
    };

  } // !misc

} // !vcsn

#endif // !VCSN_MISC_CONST_TRAITS_HH
