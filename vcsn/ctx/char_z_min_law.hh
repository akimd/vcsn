#ifndef VCSN_CTX_CHAR_Z_MIN_LAW_HH
# define VCSN_CTX_CHAR_Z_MIN_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z_min.hh>
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
    using char_z_min_law = char_<vcsn::z_min, labels_are_words>;
  }

  VCSN_CTX_INSTANTIATE(ctx::char_z_min_law);
};

#endif // !VCSN_CTX_CHAR_Z_MIN_LAW_HH
