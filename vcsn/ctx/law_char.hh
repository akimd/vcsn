#pragma once

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/labelset/wordset.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char = wordset<vcsn::set_alphabet<vcsn::char_letters>>;
  }
}
