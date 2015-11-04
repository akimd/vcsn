#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lal_char.hh>
#include <vcsn/weightset/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_r = context<lal_char, vcsn::r>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_r);
}
