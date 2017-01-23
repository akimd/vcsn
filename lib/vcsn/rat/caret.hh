#pragma once

#include <vcsn/misc/location.hh>

namespace vcsn
{
  namespace detail
  {
    void print_caret(std::istream& in,
                     std::ostream& out, const rat::location& loc);
  }
}
