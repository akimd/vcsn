#ifndef VCSN_CTX_LAL_CHAR_ZRR_HH
# define VCSN_CTX_LAL_CHAR_ZRR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z = context<Lal<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::z>;
    using lal_char_zr = context<Lal<vcsn::set_alphabet<vcsn::char_letters>>,
                                vcsn::ratexpset<lal_char_z>>;
    using lal_char_zrr = context<Lal<vcsn::set_alphabet<vcsn::char_letters>>,
                                 vcsn::ratexpset<lal_char_zr>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_zrr);
};

#endif // !VCSN_CTX_LAL_CHAR_ZRR_HH
