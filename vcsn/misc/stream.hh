#ifndef VCSN_MISC_STREAM_HH
# define VCSN_MISC_STREAM_HH

# include <sstream>
# include <stdexcept>
# include <iostream> // cin

# include <vcsn/misc/escape.hh>

namespace vcsn
{
  // Extract the string which is here betwen lbracket and rbracket.
  // Support nested lbracket/rbracket.
  std::string
  bracketed(std::istream& i, const char lbracket, const char rbracket);

  template <typename ValueSet>
  auto
  conv(const ValueSet& vs, const std::string& str)
    -> decltype(vs.conv(std::cin)) // FIXME: Fix a means to avoid cin.
  {
    std::istringstream i{str};
    auto res = vs.conv(i);
    if (i.peek() != -1)
      throw std::domain_error("invalid value: " + str
                              + ", unexpected "
                              + str_escape(i.peek()));
    return res;
  }

  /// Check lookahead character and advance.
  /// \param is the stream to read.
  /// \param c  the expected character.
  /// \throws std::runtime_error if the next character is not \a c.
  void eat(std::istream& is, char c);
}

#endif // !VCSN_MISC_STREAM_HH
