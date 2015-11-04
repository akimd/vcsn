#pragma once

#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/law_char.hh>
#include <vcsn/weightset/q.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_q = context<law_char, vcsn::q>;
  }
}
