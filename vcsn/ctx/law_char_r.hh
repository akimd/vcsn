#ifndef VCSN_CTX_LAW_CHAR_R_HH
# define VCSN_CTX_LAW_CHAR_R_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/law_char.hh>
# include <vcsn/weightset/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_r = context<law_char, vcsn::r>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::law_char_r);
};

#endif // !VCSN_CTX_LAW_CHAR_R_HH
