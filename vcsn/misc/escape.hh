#pragma once

#include <iosfwd>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Output a string, escaping special characters.
  std::ostream& str_escape(std::ostream& os, const std::string& str,
                           const char* special = nullptr);

  /// A container of letters.
  template <typename T>
  std::ostream& str_escape(std::ostream& os,
                           const std::vector<T>& s,
                           const char* special = nullptr)
  {
    for (auto c: s)
      str_escape(os, c, special);
    return os;
  }

  /// Output a character, escaping special characters.
  /// -1 denotes end-of-file.
  std::ostream& str_escape(std::ostream& os, int c,
                           const char* special = nullptr);

  /// Output a character, escaping special characters.
  std::ostream& str_escape(std::ostream& os, char c,
                           const char* special = nullptr);

  /// Convert to a string, escaped.
  template <typename T>
  std::string str_escape(T&& s,
                         const char* special = nullptr)
  {
    std::ostringstream o;
    str_escape(o, std::forward<T>(s), special);
    return o.str();
  }

  /// Convert to a string, in quotes.
  template <typename... Args>
  std::string str_quote(Args&&... args)
  {
    std::ostringstream o;
    o << '"';
    using swallow = int[];
    (void) swallow
      {
        (str_escape(o, std::forward<Args>(args), "\""), 0)...
      };
    o << '"';
    return o.str();
  }
}
