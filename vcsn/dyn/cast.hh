#pragma once

#include <utility> // std::forward

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// A dynamic_cast in debug mode, static_cast with NDEBUG.
      template <typename To, typename From>
      inline
      To dyn_cast(From&& from)
      {
#ifdef NDEBUG
        return static_cast<To>(std::forward<From>(from));
#else
        return dynamic_cast<To>(std::forward<From>(from));
#endif
      }
    }
  }
}
