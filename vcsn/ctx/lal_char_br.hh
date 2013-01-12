#ifndef VCSN_CTX_LAL_CHAR_BR_HH
# define VCSN_CTX_LAL_CHAR_BR_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = char_<vcsn::b, labels_are_letters>;
    using lal_char_br = char_<vcsn::ratexpset<lal_char_b>, labels_are_letters>;
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
