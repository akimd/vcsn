#pragma once

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{

  /// An input/output format.
  ///
  /// For instance, whether text or latex, and other syntactic issues.
  class format
  {
  public:
    using self_t = format;

    /// Type of format.
    enum format_t
      {
        text,
        latex,
        /// Print as is.  For instance, don't try to escape labels.
        raw,
        /// By default, print for text.
        deflt = text,
      };

    format(format_t f = deflt)
      : format_{f}
    {}

    format(const std::string& f);

    /// A copy of this format, but to print labels.
    format for_labels() const
    {
      format res = *this;
      res.label_ = true;
      return res;
    }

    /// A copy of this format, but to print weights.
    format for_weights() const
    {
      format res = *this;
      res.label_ = false;
      return res;
    }

    /// Whether to use the syntax for labels (e.g., "a|x").
    bool is_for_labels() const
    {
      return label_;
    }

    /// Whether to use the syntax for weights (e.g., "(1, 1/2)").
    bool is_for_weights() const
    {
      return !label_;
    }

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
    /// Whether printed as a label (e.g., "a|x"), or as a weight
    /// (e.g., "(1, 1/2)").
    bool label_ = false;
  };

  /// Wrapper around operator<<.
  std::string to_string(format i);

  /// Read from string form.
  std::istream& operator>>(std::istream& is, format& i);

  /// Output in string form.
  std::ostream& operator<<(std::ostream& os, format i);
} // namespace vcsn
