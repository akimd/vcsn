#pragma once

#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Output a string, escaping special characters.
  std::ostream& str_escape(std::ostream& os, const std::string& str,
                           const char* special = nullptr);

  /// Likewise, but produces a string.
  std::string str_escape(const std::string& c,
                         const char* special = nullptr);

  /// A container of letters.
  template <typename T>
  std::string str_escape(const std::vector<T>& s,
                         const char* special = nullptr)
  {
    std::ostringstream o;
    for (const auto& c: s)
      str_escape(o, c, special);
    return o.str();
  }

  /// Output a character, escaping special characters.
  /// -1 denotes end-of-file.
  std::ostream& str_escape(std::ostream& os, int c,
                           const char* special = nullptr);

  /// Likewise, but produces a string.
  std::string str_escape(int c,
                         const char* special = nullptr);
}
