#ifndef VCSN_CTX_CHAR_ZMIN_LAL_HH
# define VCSN_CTX_CHAR_ZMIN_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_zmin_lal = char_<vcsn::zmin, labels_are_letters>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_zmin_lal);

  MAYBE_EXTERN template
  class details::evaluator<mutable_automaton<ctx::char_zmin_lal>>;

  MAYBE_EXTERN template
  int
  eval(const mutable_automaton<ctx::char_zmin_lal>& aut, const std::string& w);
};

#endif // !VCSN_CTX_CHAR_ZMIN_LAL_HH
