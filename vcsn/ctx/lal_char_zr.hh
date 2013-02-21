#ifndef VCSN_CTX_LAL_CHAR_ZR_HH
# define VCSN_CTX_LAL_CHAR_ZR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/letterset.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z = context<letterset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::z>;
    using lal_char_zr = context<letterset<vcsn::set_alphabet<vcsn::char_letters>>,
                                vcsn::ratexpset<lal_char_z>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_zr);
};

#endif // !VCSN_CTX_LAL_CHAR_ZR_HH
