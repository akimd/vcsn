#ifndef VCSN_MISC_STATIC_HH_
#define VCSN_MISC_STATIC_HH_

namespace vcsn
{

  namespace misc
  {

    template<typename T>
    struct constify_traits
    {
      typedef const T type;
    };

    template<typename T>
    struct id_traits
    {
      typedef T type;
    };

  } // !misc

} // !vcsn

#endif // !VCSN_MISC_STATIC_HH_
