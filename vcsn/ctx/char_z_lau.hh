#ifndef VCSN_CTX_CHAR_Z_LAE_HH
# define VCSN_CTX_CHAR_Z_LAE_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_lau = char_<vcsn::z, labels_are_unit>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_z_lau);
};

#endif // !VCSN_CTX_CHAR_Z_LAE_HH
