#ifndef VCSN_CTX_CHAR_ZRR_LAL_HH
# define VCSN_CTX_CHAR_ZRR_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_lal = char_<vcsn::z, labels_are_letters>;
    using char_zr_lal = char_<vcsn::kratexpset<char_z_lal>, labels_are_letters>;
    using char_zrr_lal = char_<vcsn::kratexpset<char_zr_lal>, labels_are_letters>;
  }
}

# include <vcsn/algos/aut_to_exp.hh>
# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/standard_of.hh>
# include <vcsn/algos/transpose.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_zrr_lal);
};

#endif // !VCSN_CTX_CHAR_ZRR_LAL_HH
