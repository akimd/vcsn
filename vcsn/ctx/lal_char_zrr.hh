#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lal_char.hh>
#include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z   = context<lal_char, vcsn::z>;
    using lal_char_zr  = context<lal_char, vcsn::expressionset<lal_char_z>>;
    using lal_char_zrr = context<lal_char, vcsn::expressionset<lal_char_zr>>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_zrr);
};
