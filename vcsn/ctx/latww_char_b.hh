#ifndef VCSN_CTX_LATWW_CHAR_B_HH
# define VCSN_CTX_LATWW_CHAR_B_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/labelset/tupleset.hh>
# include <vcsn/ctx/law_char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using latww_char_b = context<tupleset<law_char, law_char>, vcsn::b>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::latww_char_b);
};

#endif // !VCSN_CTX_LATWW_CHAR_B_HH
