#ifndef VCSN_CTX_LAN_CHAR_B_HH
# define VCSN_CTX_LAN_CHAR_B_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lan_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char_b = context<lan_char, vcsn::b>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lan_char_b);
}

#endif // !VCSN_CTX_LAN_CHAR_B_HH
