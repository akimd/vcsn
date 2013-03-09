#ifndef VCSN_CTX_LAL_CHAR_R_HH
# define VCSN_CTX_LAL_CHAR_R_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_r = context<lal_char, vcsn::r>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_r);
};

#endif // !VCSN_CTX_LAL_CHAR_R_HH
