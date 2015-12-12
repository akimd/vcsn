#pragma once

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{

  /// An input/output format for valuesets.
  ///
  /// For instance, whether text or latex, and other syntactic issues.
  class format
  {
  public:
    using self_t = format;

    /// Type of format.
    enum format_t
      {
        /// Print for LaTeX.
        latex,
        /// Print as is.  For instance, don't try to escape labels.
        raw,
        /// Print as a parsable type string.
        sname,
        /// Print as plain (ASCII) text, escaped.
        text,
        /// Print as rich UTF-8 text, escaped.
        utf8,
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
      auto res = *this;
      res.label_ = true;
      return res;
    }

    /// A copy of this format, but to print weights.
    format for_weights() const
    {
      auto res = *this;
      res.label_ = false;
      return res;
    }

    /// Whether we should delimit: `(1, 2)` instead of `1, 2`.
    bool delimit() const
    {
      return delimit_;
    }

    /// Set whether we should delimit: `(1, 2)` instead of `1, 2`.
    format delimit(bool d) const
    {
      auto res = *this;
      res.delimit_ = d;
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
    /// Whether printed as a label (e.g., `a|x`), or as a weight
    /// (e.g., `(1, 1/2)`).
    bool label_ = false;
    /// Whether we should delimit.  E.g., `1, 2` or `(1, 2)`.
    bool delimit_ = false;
  };

  /// Wrapper around operator<<.
  std::string to_string(format i);

  /// Read from string form.
  std::istream& operator>>(std::istream& is, format& i);

  /// Output in string form.
  std::ostream& operator<<(std::ostream& os, format i);
} // namespace vcsn
