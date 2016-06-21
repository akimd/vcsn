#pragma once

#include <sstream>
#include <string>

#include <vcsn/misc/export.hh> // LIBVCSN_API

namespace vcsn LIBVCSN_API
{
  /// Format v via vs.print.
  template <typename ValueSet, typename Value = typename ValueSet::value_t,
            typename... Args>
  auto
  to_string(const ValueSet& vs, const Value& v, Args&&... args)
    -> std::string
  {
    std::ostringstream o;
    vs.print(v, o, std::forward<Args>(args)...);
    return o.str();
  }
}
