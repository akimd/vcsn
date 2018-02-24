#pragma once

#include <iosfwd>

#include <vcsn/misc/location.hh>

namespace vcsn::detail
{
  /// Repeat a line with an error, underlining the error with carets.
  /// Starts with `\n` to terminate the previous message.
  void print_caret(std::istream& in,
                   std::ostream& out, const rat::location& loc);

  /// Repeat a line with an error, underlining the error with carets.
  /// Print in a string.  Does not start with a `\n`.
  std::string caret(std::istream& is, const rat::location& loc);
}
