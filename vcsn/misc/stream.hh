#pragma once

#include <iostream> // cin
#include <memory> // shared_ptr
#include <sstream>
#include <stdexcept>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/to-string.hh>

namespace vcsn LIBVCSN_API
{
  /// An narrow-char stream that discards the output.
  extern std::ostream cnull;

  /// An wide-char stream that discards the output.
  extern std::wostream wcnull;

  /// Extract the string which is here between lbracket and rbracket.
  /// Support nested lbracket/rbracket.
  std::string
  bracketed(std::istream& i, char lbracket, char rbracket);

  /// Parse str via vs.conv.
  template <typename ValueSet, typename... Args>
  auto
  conv(const ValueSet& vs, const std::string& str, Args&&... args)
    -> decltype(vs.conv(std::declval<std::istream&>(),
                        std::forward<Args>(args)...))
  {
    std::istringstream i{str};
    try
      {
        const auto res = vs.conv(i, std::forward<Args>(args)...);
        VCSN_REQUIRE(i.peek() == EOF,
                     vs, ": unexpected trailing characters: ", i);
        return res;
      }
    catch (const std::runtime_error& e)
      {
        raise(e, "  while reading: ", str_escape(str));
      }
  }

  /// Check lookahead character and advance.
  /// \param is the stream to read.
  /// \param c  the expected character.
  /// \returns c
  /// \throws std::runtime_error if the next character is not \a c.
  char eat(std::istream& is, char c);

  /// Check lookahead string and advance.
  /// \param is the stream to read.
  /// \param s  the expected string.
  /// \returns s
  /// \throws std::runtime_error if the next character is not \a s.
  const std::string& eat(std::istream& is, const std::string& s);

  /// Throw an exception after failing to read from \a is.  Reset the
  /// stream to a "good" state, and read the presumably ill-formed
  /// input into a buffer string; then throw with the given \a
  /// explanation followed by the buffer string.  \a explanation
  /// should not contain trailing punctuation or spaces.
  template <typename... Args>
  ATTRIBUTE_NORETURN
  void fail_reading(std::istream& is, Args&&... args)
  {
    is.clear();
    std::string buf;
    std::getline(is, buf, '\n');
    if (!is.good())
      // This shouldn't really happen; however it's best to fail cleanly.
      is.clear();
    if (buf.empty())
      raise(std::forward<Args>(args)...);
    else
      raise(std::forward<Args>(args)..., ": ", str_escape(buf));
  }

  /// Read a single char, with possible \-escape support.
  /// EOF is an error.
  char get_char(std::istream& i);

  /// Return the contents of \a file.
  std::string get_file_contents(const std::string& file);

  /// Open \a file for reading and return its autoclosing stream.
  /// \param file   the file name.  "-" and "" denote stdin.
  std::shared_ptr<std::istream> open_input_file(const std::string& file);

  /// Open \a file for writing and return its autoclosing stream.
  /// \param file   the file name.  "-" and "" denote stdout.
  std::shared_ptr<std::ostream> open_output_file(const std::string& file);

  /// Ignore spaces.
  /// \param is the stream to read.
  void skip_space(std::istream& is);
}
