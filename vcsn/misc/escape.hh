#pragma once

#include <iosfwd>
#include <string>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Output a string, escaping special characters.
  std::ostream& str_escape(std::ostream& os, const std::string& str);

  /// Likewise, but produces a string.
  std::string str_escape(const std::string& c);

  /// Output a character, escaping special characters.
  /// -1 denotes end-of-file.
  std::ostream& str_escape(std::ostream& os, int c);

  /// Likewise, but produces a string.
  std::string str_escape(int c);
}
