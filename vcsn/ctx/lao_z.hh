#ifndef VCSN_CTX_LAO_Z_HH
# define VCSN_CTX_LAO_Z_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/oneset.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lao_z = context<oneset, vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lao_z);
};

#endif // !VCSN_CTX_LAO_Z_HH
