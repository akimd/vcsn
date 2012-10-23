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
}

#endif // !VCSN_CTX_CHAR_HH
