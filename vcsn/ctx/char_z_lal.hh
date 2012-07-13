#ifndef VCSN_CTX_CHAR_Z_LAL_HH
# define VCSN_CTX_CHAR_Z_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_lal = char_<vcsn::z, labels_are_letters>;
  }
};

#endif // !VCSN_CTX_CHAR_Z_LAL_HH
