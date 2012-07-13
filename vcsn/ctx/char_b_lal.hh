#ifndef VCSN_CTX_CHAR_B_LAL_HH
# define VCSN_CTX_CHAR_B_LAL_HH

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
    using char_b_lal = char_<vcsn::b, labels_are_letters>;
  }

  VCSN_CTX_INSTANTIATE(ctx::char_b_lal);

  MAYBE_EXTERN template
  auto
  determinize(const mutable_automaton<ctx::char_b_lal>& aut)
    -> typename std::enable_if<std::is_same<typename mutable_automaton<ctx::char_b_lal>::context_t::kind_t,
                                            labels_are_letters>::value,
                               mutable_automaton<ctx::char_b_lal>>::type;

  MAYBE_EXTERN template
  class details::evaluator<mutable_automaton<ctx::char_b_lal>,
                           typename std::enable_if<std::is_same<typename ctx::char_b_lal::kind_t,
                                                                labels_are_letters>::value>::type>;

  MAYBE_EXTERN template
  typename mutable_automaton<ctx::char_b_lal>::weight_t
  eval(const mutable_automaton<ctx::char_b_lal>& aut,
       const typename mutable_automaton<ctx::char_b_lal>::genset_t::word_t& w);


};

#endif // !VCSN_CTX_CHAR_B_LAL_HH
