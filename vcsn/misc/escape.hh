#ifndef VCSN_MISC_ESCAPE_HH
# define VCSN_MISC_ESCAPE_HH

# include <iosfwd>
# include <string>

namespace vcsn
{
  /// Output a string, escaping special characters.
  std::ostream& str_escape(std::ostream& os, const std::string& str);
}

#endif // !VCSN_MISC_ESCAPE_HH
