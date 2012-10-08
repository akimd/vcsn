#ifndef VCSN_CTX_CHAR_B_LAW_HH
# define VCSN_CTX_CHAR_B_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_b_law = char_<vcsn::b, labels_are_words>;
  }
}

# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/determinize.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/eval.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/standard_of.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_b_law);
};

#endif // !VCSN_CTX_CHAR_B_LAW_HH
