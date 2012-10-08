#ifndef VCSN_CTX_CHAR_Z_LAE_HH
# define VCSN_CTX_CHAR_Z_LAE_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_lae = char_<vcsn::z, labels_are_empty>;
  }
}

# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/standard_of.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_z_lae);
};

#endif // !VCSN_CTX_CHAR_Z_LAE_HH
