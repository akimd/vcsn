#ifndef VCSN_CTX_LAU_BR_HH
# define VCSN_CTX_LAU_BR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/letterset.hh>
# include <vcsn/ctx/unitset.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = context<letterset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::b>;
    using lau_br = context<unitset, vcsn::ratexpset<lal_char_b>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lau_br);
}

#endif // !VCSN_CTX_LAU_BR_HH
