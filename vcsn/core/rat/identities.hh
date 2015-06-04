#pragma once

#include <iostream>
#include <string>

#include <vcsn/core/join.hh>
#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  namespace rat
  {

    /// An expressionset can implement several different sets of
    /// identities on expressions.  This type represents one of those
    /// sets.
    ///
    /// Could have been a simple enum class, but having a constructor
    /// is helping to select the default identities other than the
    /// first one.
    class identities
    {
    public:
      using self_t = identities;

      enum ids_t
        {
          /// Trivial identities only.
          trivial,

          /// Trivial, plus associativity, commutatitivity, and
          /// "idempotence" for sum.
          traditional,

          /// Traditional plus series identities (distributes).
          series,
        };

      identities(ids_t id)
        : ids_{id}
      {}

      identities()
        : identities{trivial}
      {}

      ids_t ids() const
      {
        return ids_;
      }

      /// Whether traditional.
      bool is_traditional() const
      {
        return traditional <= ids_;
      }

      /// Whether series.
      bool is_series() const
      {
        return series <= ids_;
      }

      bool operator<(self_t that) const
      {
        return ids_ < that.ids_;
      }

      bool operator==(self_t that) const
      {
        return ids_ == that.ids_;
      }

      bool operator!=(self_t that) const
      {
        return !operator==(that);
      }

    private:
      ids_t ids_;
    };

    /// Wrapper around operator<<.
    std::string to_string(identities i);

    /// Read from string form.
    std::istream& operator>>(std::istream& is, identities& i);

    /// Output in string form.
    std::ostream& operator<<(std::ostream& os, identities i);

    identities meet(identities i1, identities i2);

  } // namespace rat

  namespace detail
  {
    template <>
    struct join_impl<rat::identities, rat::identities>
    {
      using type = rat::identities;
      static type join(rat::identities i1, rat::identities i2)
      {
        return std::max(i1, i2);
      }
    };
  }
} // namespace vcsn
