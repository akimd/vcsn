#ifndef VCSN_MISC_STREAM_HH
# define VCSN_MISC_STREAM_HH

# include <sstream>

namespace vcsn
{
  // Extract the string which is here betwen lbracket and rbracket.
  // Support nested lbracket/rbracket.
  std::string
  bracketed(std::istream& i, const char lbracket, const char rbracket);
}

#endif // !VCSN_MISC_STREAM_HH
