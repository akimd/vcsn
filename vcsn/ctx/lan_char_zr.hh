#ifndef VCSN_CTX_LAN_CHAR_ZR_HH
# define VCSN_CTX_LAN_CHAR_ZR_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/ctx/lan_char.hh>
# include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z  = context<lal_char, vcsn::z>;
    using lan_char_zr = context<lan_char, vcsn::ratexpset<lal_char_z>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lan_char_zr);
};

#endif // !VCSN_CTX_LAN_CHAR_ZR_HH
