#ifndef VCSN_CTX_CHAR_ZRR_LAL_HH
# define VCSN_CTX_CHAR_ZRR_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_z_lal = char_<vcsn::z, labels_are_letters>;
    using char_zr_lal = char_<vcsn::ratexpset<char_z_lal>, labels_are_letters>;
    using char_zrr_lal = char_<vcsn::ratexpset<char_zr_lal>, labels_are_letters>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_zrr_lal);
};

#endif // !VCSN_CTX_CHAR_ZRR_LAL_HH
