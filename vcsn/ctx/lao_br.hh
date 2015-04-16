#pragma once

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b
      = context<letterset<vcsn::set_alphabet<vcsn::char_letters>>,
                vcsn::b>;
    using lao_br = context<oneset, vcsn::expressionset<lal_char_b>>;
  }
}

#include <vcsn/ctx/instantiate.hh>

#ifndef MAYBE_EXTERN
# define MAYBE_EXTERN extern
#endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lao_br);
}
