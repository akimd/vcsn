#ifndef VCSN_CTX_CHAR_B_LAW_HH
# define VCSN_CTX_CHAR_B_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/standard_of.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  namespace ctx
  {
    using char_b_law = char_<vcsn::b, labels_are_words>;
  }

  VCSN_CTX_INSTANTIATE(ctx::char_b_law);
};

#endif // !VCSN_CTX_CHAR_B_LAW_HH
