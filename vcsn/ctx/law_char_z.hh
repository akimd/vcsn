#ifndef VCSN_CTX_LAW_CHAR_Z_HH
# define VCSN_CTX_LAW_CHAR_Z_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/wordset.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z = context<WordSet<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_z);
};

#endif // !VCSN_CTX_LAW_CHAR_Z_HH
