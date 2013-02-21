#ifndef VCSN_CTX_LAW_CHAR_R_HH
# define VCSN_CTX_LAW_CHAR_R_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/wordset.hh>
# include <vcsn/weights/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_r = context<wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::r>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_r);
};

#endif // !VCSN_CTX_LAW_CHAR_R_HH
