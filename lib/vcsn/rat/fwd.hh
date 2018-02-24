#pragma once

#include <set>
#include <string>

namespace vcsn::rat
{
  /// A set of label ranges.
  using class_t = std::set<std::pair<std::string, std::string>>;

  // lib/vcsn/rat/driver.hh.
  class driver;

  // lib/vcsn/rat/scan.hh.
  class yyFlexLexer;

  // lib/vcsn/rat/location.hh.
  class location;
}
