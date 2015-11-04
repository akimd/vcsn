#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/law_char.hh>
#include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_z  = context<law_char, vcsn::z>;
    using law_char_zr = context<law_char, vcsn::expressionset<law_char_z>>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::law_char_zr);
};
