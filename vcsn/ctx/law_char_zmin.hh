#ifndef VCSN_CTX_LAW_CHAR_ZMIN_HH
# define VCSN_CTX_LAW_CHAR_ZMIN_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_zmin = char_<vcsn::zmin, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_zmin);
};

#endif // !VCSN_CTX_LAW_CHAR_ZMIN_HH
