#pragma once

#include <iosfwd>
#include <string>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Orientation.
  enum class direction
  {
    /// Looking downstream.
    forward,
    /// Looking upstream.
    backward,
  };

  /// Conversion to string.
  std::string to_string(direction d);

  /// Parsing.
  std::istream& operator>>(std::istream& is, direction& d);

  /// Pretty-printing.
  std::ostream& operator<<(std::ostream& os, direction d);
};
