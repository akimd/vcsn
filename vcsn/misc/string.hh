#pragma once

#include <algorithm>
#include <string>

namespace vcsn
{
  /// Whether is a UTF-8 continuation
  inline bool iscont(int c)
  {
    return (c & 0xc0) == 0x80;
  }

  /// Number of actual UTF-8 characters, i.e., number of
  /// non-continuation bytes.
  template <typename Iterator>
  inline int length(Iterator begin, Iterator end)
  {
    return std::count_if(begin, end,
                         [](char c) { return !iscont(c); });
  }

  /// Number of actual UTF-8 characters, i.e., number of
  /// non-continuation bytes.
  inline int length(std::string_view r)
  {
    using std::cbegin;
    using std::cend;
    return length(cbegin(r), cend(r));
  }
}
