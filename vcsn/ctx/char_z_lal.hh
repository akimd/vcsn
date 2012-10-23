#ifndef VCSN_CTX_CHAR_Z_LAL_HH
# define VCSN_CTX_CHAR_Z_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_lal = char_<vcsn::z, labels_are_letters>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_z_lal);

  MAYBE_EXTERN template
  class details::evaluator<mutable_automaton<ctx::char_z_lal>>;

  MAYBE_EXTERN template
  int
  eval(const mutable_automaton<ctx::char_z_lal>& aut, const std::string& w);

# if VCSN_INSTANTIATION
  namespace
  {
    static bool register_char_z_lal =
      dyn::details::eval_register
      (mutable_automaton<ctx::char_z_lal>::sname(),
       dyn::details::eval<mutable_automaton<ctx::char_z_lal>>)
      ;
  }
#endif
};

#endif // !VCSN_CTX_CHAR_Z_LAL_HH
