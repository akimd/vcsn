#ifndef VCSN_CTX_LAL_CHAR_HH
# define VCSN_CTX_LAL_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/labelset/letterset.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char = letterset<vcsn::set_alphabet<vcsn::char_letters>>;
  }
}

#endif // !VCSN_CTX_LAL_CHAR_HH
