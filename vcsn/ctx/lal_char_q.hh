#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lal_char.hh>
#include <vcsn/weightset/q.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_q = context<lal_char, vcsn::q>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_q);
}
