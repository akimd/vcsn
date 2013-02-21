#ifndef VCSN_CTX_LAW_CHAR_ZRR_HH
# define VCSN_CTX_LAW_CHAR_ZRR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/wordset.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z = context<wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::z>;
    using law_char_zr = context<wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                                vcsn::ratexpset<law_char_z>>;
    using law_char_zrr = context<wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                                 vcsn::ratexpset<law_char_zr>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_zrr);
};

#endif // !VCSN_CTX_LAW_CHAR_ZRR_HH
