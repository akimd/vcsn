#ifndef VCSN_CTX_LAW_CHAR_BR_HH
# define VCSN_CTX_LAW_CHAR_BR_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_b = char_<vcsn::b, labels_are_words>;
    using law_char_br = char_<vcsn::ratexpset<law_char_b>, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_br);
};

#endif // !VCSN_CTX_LAW_CHAR_BR_HH
