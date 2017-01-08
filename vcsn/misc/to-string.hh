#pragma once

#include <iostream> // std::cout
#include <sstream>
#include <string>

namespace vcsn
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

  /// Format vs via print_set.
  template <typename ValueSet>
  auto
  to_string(const ValueSet& vs)
    -> decltype(vs.print_set(std::cout), std::string{})
  {
    std::ostringstream o;
    vs.print_set(o);
    return o.str();
  }
}
