#ifndef VCSN_CTX_LAN_CHAR_HH
# define VCSN_CTX_LAN_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/labelset/nullableset.hh>
# include <vcsn/ctx/lal_char.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char = nullableset<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>>;
  }
}

#endif // !VCSN_CTX_LAN_CHAR_HH
