#ifndef VCSN_MISC_STREAM_HH
# define VCSN_MISC_STREAM_HH

# include <sstream>
# include <stdexcept>
# include <iostream> // cin
# include <memory> // shared_ptr

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// An narrow-char stream that discards the output.
  extern std::ostream cnull;

  /// An wide-char stream that discards the output.
  extern std::wostream wcnull;

  /// Extract the string which is here betwen lbracket and rbracket.
  /// Support nested lbracket/rbracket.
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

  /// Check lookahead string and advance.
  /// \param is the stream to read.
  /// \param s  the expected string.
  /// \throws std::runtime_error if the next character is not \a s.
  void eat(std::istream& is, const std::string& s);

  /// Return the contents of \a file.
  std::string get_file_contents(const std::string& file);

  /// Open \a file for reading and return its autoclosing stream.
  /// \param file   the file name.  "-" and "" denote stdin.
  std::shared_ptr<std::istream> open_input_file(const std::string& file);

  /// Open \a file for writing and return its autoclosing stream.
  /// \param file   the file name.  "-" and "" denote stdout.
  std::shared_ptr<std::ostream> open_output_file(const std::string& file);

}

#endif // !VCSN_MISC_STREAM_HH
