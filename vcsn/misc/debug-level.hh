#pragma once

#include <cstdlib>
#include <boost/lexical_cast.hpp>

namespace vcsn
{
  namespace detail
  {
    /// Debug level set in the user's environment.
    static inline int debug_level()
    {
      if (auto cp = getenv("VCSN_DEBUG"))
        return  *cp ? boost::lexical_cast<int>(cp) : 1;
      else
        return 0;
    }
  }
}
