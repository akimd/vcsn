#ifndef VCSN_CTX_LAW_CHAR_ZMIN_HH
# define VCSN_CTX_LAW_CHAR_ZMIN_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/wordset.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_zmin = context<WordSet<vcsn::set_alphabet<vcsn::char_letters>>,
                                  vcsn::zmin>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_zmin);
};

#endif // !VCSN_CTX_LAW_CHAR_ZMIN_HH
