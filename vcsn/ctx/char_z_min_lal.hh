#ifndef VCSN_CTX_CHAR_Z_MIN_LAL_HH
# define VCSN_CTX_CHAR_Z_MIN_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z_min.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_min_lal = char_<vcsn::z_min, labels_are_letters>;
  }
};

#endif // !VCSN_CTX_CHAR_Z_MIN_LAL_HH
