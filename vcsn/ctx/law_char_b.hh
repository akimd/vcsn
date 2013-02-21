#ifndef VCSN_CTX_LAW_CHAR_B_HH
# define VCSN_CTX_LAW_CHAR_B_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/wordset.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_b = context<wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::b>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_b);
};

#endif // !VCSN_CTX_LAW_CHAR_B_HH
