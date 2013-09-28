#ifndef VCSN_CTX_LAW_CHAR_Q_HH
# define VCSN_CTX_LAW_CHAR_Q_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/law_char.hh>
# include <vcsn/weights/q.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_q = context<law_char, vcsn::q>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_q);
};

#endif // !VCSN_CTX_LAW_CHAR_Q_HH
