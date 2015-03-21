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
  /// Useful for variadic operator on a single argument.
  template <typename ValueSet>
  auto
  join(const ValueSet& vs)
    -> ValueSet
  {
    return vs;
  }

  /// The join of two (or more) valuesets.
  template <typename ValueSet1, typename ValueSet2, typename... VSs>
  auto
  join(const ValueSet1& vs1, const ValueSet2& vs2, const VSs&... vs)
    -> decltype(join(detail::join_(vs1, vs2, 0), vs...))
  {
    return join(detail::join_(vs1, vs2, 0), vs...);
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
