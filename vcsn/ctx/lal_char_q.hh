#ifndef VCSN_CTX_LAL_CHAR_Q_HH
# define VCSN_CTX_LAL_CHAR_Q_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/q.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_q = context<lal_char, vcsn::q>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_q);
};

#endif // !VCSN_CTX_LAL_CHAR_Q_HH

