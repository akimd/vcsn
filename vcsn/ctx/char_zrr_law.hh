#ifndef VCSN_CTX_CHAR_ZRR_LAW_HH
# define VCSN_CTX_CHAR_ZRR_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_law = char_<vcsn::z, labels_are_words>;
    using char_zr_law = char_<vcsn::ratexpset<char_z_law>, labels_are_words>;
    using char_zrr_law = char_<vcsn::ratexpset<char_zr_law>, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_zrr_law);
};

#endif // !VCSN_CTX_CHAR_ZRR_LAW_HH
