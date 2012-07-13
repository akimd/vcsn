#ifndef VCSN_CTX_CHAR_HH
# define VCSN_CTX_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>

namespace vcsn
{
  namespace ctx
  {
    template <typename WeightSet, typename Kind = labels_are_letters>
    using char_ = context<vcsn::set_alphabet<vcsn::char_letters>,
                          WeightSet,
                          Kind>;
  }

#define VCSN_CTX_INSTANTIATE(Ctx)                                       \
  MAYBE_EXTERN template                                                 \
  class polynomials<Ctx>;                                               \
                                                                        \
  MAYBE_EXTERN template                                                 \
  class mutable_automaton<Ctx>;                                         \
                                                                        \
  MAYBE_EXTERN template                                                 \
  void                                                                  \
  dotty<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut,      \
                                std::ostream& out);                     \
                                                                        \
  MAYBE_EXTERN template                                                 \
  std::string                                                           \
  dotty<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);     \
                                                                        \
  MAYBE_EXTERN template                                                 \
  details::lifted_automaton_t<mutable_automaton<Ctx>>                   \
  lift<mutable_automaton<Ctx>>(const mutable_automaton<Ctx>& aut);      \
                                                                        \
  MAYBE_EXTERN template                                                 \
  class rat::standard_of_visitor<mutable_automaton<Ctx>>;


}

#endif // !VCSN_CTX_CHAR_HH
