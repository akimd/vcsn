#ifndef VCSN_CORE_RAT_IDENTITIES_HH
# define VCSN_CORE_RAT_IDENTITIES_HH

# include <iostream>
# include <string>

# include <vcsn/core/join.hh>
# include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  namespace rat
  {

    /// A ratexpset can implement several different sets of identities on
    /// expressions.  This type represents one of those sets.
    enum class identities
    {
      /// Trivial identities only.
      trivial,

      /// Trivial identities plus series identities.
      series,
    };

    std::string to_string(identities i);
    std::istream& operator>>(std::istream& is, identities& i);
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

#endif // !VCSN_CORE_RAT_IDENTITIES_HH
