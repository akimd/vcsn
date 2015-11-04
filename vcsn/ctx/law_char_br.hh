#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/law_char.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_b  = context<law_char, vcsn::b>;
    using law_char_br = context<law_char, vcsn::expressionset<law_char_b>>;
  }
}

#include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::law_char_br);
};
