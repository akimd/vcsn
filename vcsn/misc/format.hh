#pragma once

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  class format
  {
  public:
    using self_t = format;

    enum format_t
      {
        text,
        latex,
        deflt = text
      };

    format(format_t f = deflt)
      : format_{f}
    {}

    format(const std::string& f);

    format_t kind() const
    {
      return format_;
    }

    bool operator<(self_t that) const
    {
      return format_ < that.format_;
    }

    bool operator==(self_t that) const
    {
      return format_ == that.format_;
    }

    bool operator!=(self_t that) const
    {
      return !operator==(that);
    }

  private:
    format_t format_;
  };

  /// Wrapper around operator<<.
  std::string to_string(format i);

  /// Read from string form.
  std::istream& operator>>(std::istream& is, format& i);

  /// Output in string form.
  std::ostream& operator<<(std::ostream& os, format i);
} // namespace vcsn
