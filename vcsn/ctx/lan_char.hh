#ifndef VCSN_CTX_LAN_CHAR_HH
# define VCSN_CTX_LAN_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/nullableset.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char = nullableset<vcsn::set_alphabet<vcsn::char_letters>>;
  }
}

#endif // !VCSN_CTX_LAN_CHAR_HH
