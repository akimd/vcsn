#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lan_char.hh>
#include <vcsn/weightset/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char_r = context<lan_char, vcsn::r>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lan_char_r);
};
