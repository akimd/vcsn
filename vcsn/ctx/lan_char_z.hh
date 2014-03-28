#ifndef VCSN_CTX_LAN_CHAR_Z_HH
# define VCSN_CTX_LAN_CHAR_Z_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lan_char.hh>
# include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char_z = context<lan_char, vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lan_char_z);
};

#endif // !VCSN_CTX_LAN_CHAR_Z_HH
