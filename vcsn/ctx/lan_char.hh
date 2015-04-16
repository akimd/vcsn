#pragma once

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/labelset/nullableset.hh>

namespace vcsn
{
  namespace ctx
  {
    using lan_char
      = nullableset<vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>>;
  }
}
