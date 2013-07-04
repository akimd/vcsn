#ifndef VCSN_CTX_LAW_CHAR_HH
# define VCSN_CTX_LAW_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/labelset/wordset.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char = wordset<vcsn::set_alphabet<vcsn::char_letters>>;
  }
}

#endif // !VCSN_CTX_LAW_CHAR_HH
