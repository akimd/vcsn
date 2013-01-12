#ifndef VCSN_CTX_LAW_CHAR_ZRR_HH
# define VCSN_CTX_LAW_CHAR_ZRR_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z = char_<vcsn::z, labels_are_words>;
    using law_char_zr = char_<vcsn::ratexpset<law_char_z>, labels_are_words>;
    using law_char_zrr = char_<vcsn::ratexpset<law_char_zr>, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_zrr);
};

#endif // !VCSN_CTX_LAW_CHAR_ZRR_HH
