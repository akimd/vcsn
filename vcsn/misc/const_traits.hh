#ifndef VCSN_MISC_CONST_TRAITS_HH
# define VCSN_MISC_CONST_TRAITS_HH

namespace vcsn
{

  namespace misc
  {

    template <typename T>
    struct constify_traits
    {
      typedef const T type;
    };

    template <typename T>
    struct id_traits
    {
      typedef T type;
    };

  } // !misc

} // !vcsn

#endif // !VCSN_MISC_CONST_TRAITS_HH
