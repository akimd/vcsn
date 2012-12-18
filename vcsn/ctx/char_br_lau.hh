#ifndef VCSN_CTX_CHAR_BR_LAU_HH
# define VCSN_CTX_CHAR_BR_LAU_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_b_lal = char_<vcsn::b, labels_are_letters>;
    using char_br_lau = char_<vcsn::ratexpset<char_b_lal>, labels_are_unit>;
  }
}

# include <vcsn/ctx/instantiate.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_br_lau);
}

#endif // !VCSN_CTX_CHAR_BR_LAU_HH
