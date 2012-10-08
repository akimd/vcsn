#ifndef VCSN_CTX_CHAR_ZMIN_LAW_HH
# define VCSN_CTX_CHAR_ZMIN_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/zmin.hh>
namespace vcsn
{
  namespace ctx
  {
    using char_zmin_law = char_<vcsn::zmin, labels_are_words>;
  }
}

# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/algos/dotty.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/make-context.hh>
# include <vcsn/algos/standard_of.hh>
# include <vcsn/algos/transpose.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_zmin_law);
};

#endif // !VCSN_CTX_CHAR_ZMIN_LAW_HH
