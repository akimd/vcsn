#ifndef VCSN_CTX_LAW_CHAR_BR_HH
# define VCSN_CTX_LAW_CHAR_BR_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/law_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_b  = context<law_char, vcsn::b>;
    using law_char_br = context<law_char, vcsn::ratexpset<law_char_b>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_br);
};

#endif // !VCSN_CTX_LAW_CHAR_BR_HH
