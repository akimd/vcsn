#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/law_char.hh>
#include <vcsn/weightset/r.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_r = context<law_char, vcsn::r>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::law_char_r);
};
