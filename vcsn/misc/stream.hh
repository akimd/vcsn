#pragma once

#include <sstream>
#include <stdexcept>
#include <iostream> // cin
#include <memory> // shared_ptr

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/raise.hh>

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
  inline auto
  conv(const ValueSet& vs, const std::string& str, Args&&... args)
    -> decltype(vs.conv(std::declval<std::istream&>(),
                        std::forward<Args>(args)...))
  {
    std::istringstream i{str};
    auto res = vs.conv(i, std::forward<Args>(args)...);
    VCSN_REQUIRE(i.peek() == EOF,
                 vs.sname(), ": invalid value: ", str,
                 ", unexpected ", str_escape(i.peek()));
    return res;
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
  inline
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
  inline char get_char(std::istream& i)
  {
    int res = i.get();
    if (res == '\\')
      {
        int c = i.peek();
        // \(, \) and \- are used in setalpha::make, e.g.,
        // char_letters(\(\-\)), so strip the backslash.  Otherwise,
        // return the backslash itself.
        if (c == 'x')
          {
            i.ignore();
            // Handle hexadecimal escape.
            int c1 = i.get();
            require(c1 != EOF,
                    "get_char: unexpected end-of-file"
                    " after: \\x");
            require(isxdigit(c1),
                    "get_char: invalid escape: \\x", char(c1));
            int c2 = i.get();
            require(c2 != EOF,
                    "get_char: unexpected end-of-file"
                    " after: \\x", char(c1));
            require(isxdigit(c2),
                    "get_char: invalid escape: \\x",
                    char(c1), char(c2));
            res = std::stoi(std::string{char(c1), char(c2)}, nullptr, 16);
          }
        else if (std::isalnum(c))
          raise("get_char: invalid escape: \\", char(c), " in \\", i);
        else
          res = i.get();
      }
    require(res != EOF,
            "get_char: unexpected end-of-file");
    return res;
  }

  /// Format v via vs.print.
  template <typename ValueSet, typename Value = typename ValueSet::value_t,
            typename... Args>
  inline auto
  to_string(const ValueSet& vs, const Value& v, Args&&... args)
    -> std::string
  {
    std::ostringstream o;
    vs.print(v, o, std::forward<Args>(args)...);
    return o.str();
  }

  /// Return the contents of \a file.
  std::string get_file_contents(const std::string& file);

  /// Open \a file for reading and return its autoclosing stream.
  /// \param file   the file name.  "-" and "" denote stdin.
  std::shared_ptr<std::istream> open_input_file(const std::string& file);

  /// Open \a file for writing and return its autoclosing stream.
  /// \param file   the file name.  "-" and "" denote stdout.
  std::shared_ptr<std::ostream> open_output_file(const std::string& file);

}
