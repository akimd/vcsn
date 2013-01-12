#ifndef VCSN_CTX_LAW_CHAR_B_HH
# define VCSN_CTX_LAW_CHAR_B_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_b = char_<vcsn::b, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_b);
};

#endif // !VCSN_CTX_LAW_CHAR_B_HH
