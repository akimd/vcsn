#ifndef VCSN_CTX_CHAR_ZMIN_LAW_HH
# define VCSN_CTX_CHAR_ZMIN_LAW_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_zmin_law = char_<vcsn::zmin, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_zmin_law);
};

#endif // !VCSN_CTX_CHAR_ZMIN_LAW_HH
