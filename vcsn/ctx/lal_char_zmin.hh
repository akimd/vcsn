#ifndef VCSN_CTX_LAL_CHAR_ZMIN_HH
# define VCSN_CTX_LAL_CHAR_ZMIN_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_zmin = char_<vcsn::zmin, labels_are_letters>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_zmin);

  MAYBE_EXTERN template
  class details::evaluator<mutable_automaton<ctx::lal_char_zmin>>;

  MAYBE_EXTERN template
  int
  eval(const mutable_automaton<ctx::lal_char_zmin>& aut, const std::string& w);
};

#endif // !VCSN_CTX_LAL_CHAR_ZMIN_HH
