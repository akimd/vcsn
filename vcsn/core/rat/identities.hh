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
          /// Strictly obey to the syntax.
          none,

          /// Trivial identities only.
          trivial,

          /// Trivial, plus associativity of add and product.
          associative,

          /// Associative plus commutativity, and "idempotence" for
          /// add.
          linear,

          /// Linear plus distribution.  Used for series identities.
          distributive,

          /// Distributive plus optimizations.
          agressive,

          /// The default value.
          deflt = linear,
        };

      identities(ids_t id = deflt)
        : ids_{id}
      {}

      /// Build from a string.
      identities(const std::string& i);

      /// Build from a literal string.
      identities(const char* cp);

      ids_t ids() const
      {
        return ids_;
      }

      /// Whether agressive optimizations are on.
      bool is_agressive() const
      {
        return agressive <= ids_;
      }

      /// Whether associative identities are on.
      bool is_associative() const
      {
        return associative <= ids_;
      }

      /// Whether distributive identities are on.
      bool is_distributive() const
      {
        return ids_ == distributive;
      }

      /// Whether linear identities are on.
      bool is_linear() const
      {
        return linear <= ids_;
      }

      /// Whether trivial identities are on.
      bool is_trivial() const
      {
        return trivial <= ids_;
      }

      /// Whether not none.
      operator bool() const
      {
        return ids_ != none;
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

    /// More restricted of these identities (min).
    identities meet(identities i1, identities i2);
  } // namespace rat

  namespace detail
  {
    /// More general of these identities (max).
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
