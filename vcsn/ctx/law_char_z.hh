#ifndef VCSN_CTX_LAW_CHAR_Z_HH
# define VCSN_CTX_LAW_CHAR_Z_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/law_char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z = context<law_char, vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_z);
};

#endif // !VCSN_CTX_LAW_CHAR_Z_HH
