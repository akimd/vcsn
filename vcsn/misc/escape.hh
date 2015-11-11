#pragma once

#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Output a string, escaping special characters.
  std::ostream& str_escape(std::ostream& os, const std::string& str);

  /// Likewise, but produces a string.
  std::string str_escape(const std::string& c);

  /// A container of letters.
  template <typename T>
  std::string str_escape(const std::vector<T>& s)
  {
    std::ostringstream o;
    for (const auto& c: s)
      str_escape(o, c);
    return o.str();
  }

  /// Output a character, escaping special characters.
  /// -1 denotes end-of-file.
  std::ostream& str_escape(std::ostream& os, int c);

  /// Likewise, but produces a string.
  std::string str_escape(int c);
}
