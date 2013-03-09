#ifndef VCSN_CTX_LAL_CHAR_BR_HH
# define VCSN_CTX_LAL_CHAR_BR_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b  = context<lal_char, vcsn::b>;
    using lal_char_br = context<lal_char, vcsn::ratexpset<lal_char_b>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_br);
};

#endif // !VCSN_CTX_LAL_CHAR_BR_HH
