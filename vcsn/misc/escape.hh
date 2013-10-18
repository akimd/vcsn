#ifndef VCSN_MISC_ESCAPE_HH
# define VCSN_MISC_ESCAPE_HH

# include <iosfwd>
# include <string>

# include <vcsn/misc/export.hh>

namespace vcsn
{
  /// Output a string, escaping special characters.
  LIBVCSN_API
  std::ostream& str_escape(std::ostream& os, const std::string& str);

  /// Likewise, but produces a string.
  LIBVCSN_API
  std::string str_escape(const std::string& c);

  /// Output a character, escaping special characters.
  /// -1 denotes end-of-file.
  LIBVCSN_API
  std::ostream& str_escape(std::ostream& os, int c);

  /// Likewise, but produces a string.
  LIBVCSN_API
  std::string str_escape(int c);
}

#endif // !VCSN_MISC_ESCAPE_HH
