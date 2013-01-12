#ifndef VCSN_CTX_LAW_CHAR_Z_HH
# define VCSN_CTX_LAW_CHAR_Z_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z = char_<vcsn::z, labels_are_words>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_z);
};

#endif // !VCSN_CTX_LAW_CHAR_Z_HH
