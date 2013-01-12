#ifndef VCSN_CTX_LAL_CHAR_ZRR_HH
# define VCSN_CTX_LAL_CHAR_ZRR_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z = char_<vcsn::z, labels_are_letters>;
    using lal_char_zr = char_<vcsn::ratexpset<lal_char_z>, labels_are_letters>;
    using lal_char_zrr = char_<vcsn::ratexpset<lal_char_zr>, labels_are_letters>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_zrr);
};

#endif // !VCSN_CTX_LAL_CHAR_ZRR_HH
