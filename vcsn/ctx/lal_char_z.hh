#ifndef VCSN_CTX_LAL_CHAR_Z_HH
# define VCSN_CTX_LAL_CHAR_Z_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z = context<lal_char, vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_z);
};

#endif // !VCSN_CTX_LAL_CHAR_Z_HH
