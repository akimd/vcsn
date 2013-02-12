#ifndef VCSN_CTX_Z_LAU_HH
# define VCSN_CTX_Z_LAU_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/unitset.hh>
# include <vcsn/weights/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lau_z = context<UnitSet, vcsn::z>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lau_z);
};

#endif // !VCSN_CTX_Z_LAU_HH
