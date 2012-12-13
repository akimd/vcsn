#ifndef VCSN_CTX_CHAR_BR_LAW_HH
# define VCSN_CTX_CHAR_BR_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_b_law = char_<vcsn::b, labels_are_words>;
    using char_br_law = char_<vcsn::ratexpset<char_b_law>, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_br_law);
};

#endif // !VCSN_CTX_CHAR_BR_LAW_HH
