#pragma once

#include <vcsn/misc/type_traits.hh>

namespace vcsn
{
  namespace detail
  {

    /// A structure that implements the computation of join(V1, V2).
    /// "type" is the type of the result, and "join" computes the
    /// value.
    ///
    /// "Enable" is used to discard implementation we want to
    /// eliminate (e.g., to avoid ambiguous specializations).
    template <typename V1, typename V2,
              typename Enable = void>
    struct join_impl
    {};

    /// Dealing with commutativity: two implementations of join_:
    /// forward and reversed, ordered by preference by the use of "0
    /// prefers int to long" disambiguation rule.
    template <typename V1, typename V2>
    typename join_impl<V1, V2>::type
    join_(V1 v1, V2 v2, int)
    {
      return join_impl<V1, V2>::join(v1, v2);
    }

    template <typename V1, typename V2>
    typename join_impl<V1, V2>::type
    join_(V2 v2, V1 v1, long)
    {
      return join_impl<V1, V2>::join(v1, v2);
    }
  }

  /// The join of a single valueset.
  ///
  /// Useful for variadic operator on a single argument.
  template <typename ValueSet>
  auto
  join(const ValueSet& vs)
    -> ValueSet
  {
    return vs;
  }

  /// The join of two valuesets.
  template <typename ValueSet1, typename ValueSet2>
  auto
  join(ValueSet1 vs1, ValueSet2 vs2)
    -> decltype(detail::join_(vs1, vs2, 0))
  {
    return detail::join_(vs1, vs2, 0);
  }

  /// The join of three (or more) valuesets.
  ///
  /// We once had a single case for two-and-more arguments (instead of
  /// one join for two and one join for three-and-more), but the
  /// decltype() used in the return type failed to find the
  /// appropriate 'recursive' call to join in the case of three
  /// rat::identities.  It turns out that removing the return type
  /// (and leaving its computation to the C++14 compiler) suffices to
  /// force the instantiation of the function, and to fix the problem.
  template <typename ValueSet1, typename ValueSet2, typename ValueSet3,
            typename... VSs>
  auto
  join(ValueSet1 vs1, ValueSet2 vs2, ValueSet3 vs3, VSs&&... vs)
  {
    return join(join(vs1, vs2), vs3, std::forward<VSs>(vs)...);
  }

  /// The type of the join of the \a ValueSets.
  template <typename... ValueSets>
  using join_t = decltype(join(std::declval<ValueSets>()...));
}

/// Declare that Lhs v Rhs => Rhs.
///
/// Call this from the vcsn::detail namespace.
#define VCSN_JOIN_SIMPLE(Lhs, Rhs)              \
  /** The join of two simple valuesets. */      \
  template <>                                   \
  struct join_impl<Lhs, Rhs>                    \
  {                                             \
    using type = Rhs;                           \
    static type join(Lhs, Rhs)                  \
    {                                           \
      return {};                                \
    }                                           \
  }
