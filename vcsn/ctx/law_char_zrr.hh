#ifndef VCSN_CTX_LAW_CHAR_ZRR_HH
# define VCSN_CTX_LAW_CHAR_ZRR_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/law_char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z   = context<law_char, vcsn::z>;
    using law_char_zr  = context<law_char, vcsn::ratexpset<law_char_z>>;
    using law_char_zrr = context<law_char, vcsn::ratexpset<law_char_zr>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::law_char_zrr);
};

#endif // !VCSN_CTX_LAW_CHAR_ZRR_HH
