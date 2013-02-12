#ifndef VCSN_CTX_LAL_CHAR_Z_HH
# define VCSN_CTX_LAL_CHAR_Z_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/letterset.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z = context<LetterSet<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_z);

  MAYBE_EXTERN template
  class details::evaluator<mutable_automaton<ctx::lal_char_z>>;

  MAYBE_EXTERN template
  int
  eval(const mutable_automaton<ctx::lal_char_z>& aut, const std::string& w);
};

#endif // !VCSN_CTX_LAL_CHAR_Z_HH
