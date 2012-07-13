#ifndef VCSN_CTX_CHAR_B_LAL_HH
# define VCSN_CTX_CHAR_B_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>
# include <vcsn/algos/lift.hh>
# include <vcsn/algos/dotty.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  namespace ctx
  {
    using char_b_lal = char_<vcsn::b, labels_are_letters>;
  }

  MAYBE_EXTERN template
  class polynomials<ctx::char_b_lal>;

  MAYBE_EXTERN template
  class mutable_automaton<ctx::char_b_lal>;

  MAYBE_EXTERN template
  void
  dotty<mutable_automaton<ctx::char_b_lal>>
  (const mutable_automaton<ctx::char_b_lal>& aut, std::ostream& out);

  MAYBE_EXTERN template
  std::string
  dotty<mutable_automaton<ctx::char_b_lal>>
  (const mutable_automaton<ctx::char_b_lal>& aut);

  MAYBE_EXTERN template
  details::lifted_automaton_t<mutable_automaton<ctx::char_b_lal>>
  lift<mutable_automaton<ctx::char_b_lal>>
  (const mutable_automaton<ctx::char_b_lal>& aut);

};

#endif // !VCSN_CTX_CHAR_B_LAL_HH
