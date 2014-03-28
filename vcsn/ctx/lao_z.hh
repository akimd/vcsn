#ifndef VCSN_CTX_LAO_Z_HH
# define VCSN_CTX_LAO_Z_HH

# include <vcsn/labelset/oneset.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/weightset/z.hh>

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
  VCSN_CTX_INSTANTIATE(ctx::lao_z);
};

#endif // !VCSN_CTX_LAO_Z_HH
