#ifndef VCSN_CTX_CHAR_Z_LAL_HH
# define VCSN_CTX_CHAR_Z_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/standard_of.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  namespace ctx
  {
    using char_z_lal = char_<vcsn::z, labels_are_letters>;
  }

  VCSN_CTX_INSTANTIATE(ctx::char_z_lal);
};

#endif // !VCSN_CTX_CHAR_Z_LAL_HH
