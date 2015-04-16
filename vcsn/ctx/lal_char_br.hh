#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lal_char.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b  = context<lal_char, vcsn::b>;
    using lal_char_br = context<lal_char, vcsn::expressionset<lal_char_b>>;
  }
}

#include <vcsn/ctx/instantiate.hh>

#ifndef MAYBE_EXTERN
# define MAYBE_EXTERN extern
#endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_br);
}
