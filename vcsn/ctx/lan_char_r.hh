#ifndef VCSN_CTX_LAN_CHAR_R_HH
# define VCSN_CTX_LAN_CHAR_R_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lan_char.hh>
# include <vcsn/weights/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char_r = context<lan_char, vcsn::r>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lan_char_r);
};

#endif // !VCSN_CTX_LAN_CHAR_R_HH
