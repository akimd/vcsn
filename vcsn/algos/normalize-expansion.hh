#pragma once

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*--------------.
  | Expansions.   |
  `--------------*/

  /// Denormalize an expansion.
  template <typename ExpansionSet>
  typename ExpansionSet::value_t
  denormalize(const ExpansionSet& xs, typename ExpansionSet::value_t x)
  {
    return xs.denormalize(x);
  }

  /// Normalize an expansion.
  template <typename ExpansionSet>
  bool
  is_normal(const ExpansionSet& xs, const typename ExpansionSet::value_t& x)
  {
    return xs.is_normal(x);
  }

  /// Normalize an expansion.
  template <typename ExpansionSet>
  typename ExpansionSet::value_t
  normalize(const ExpansionSet& xs, typename ExpansionSet::value_t x)
  {
    return xs.normalize(x);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpansionSet>
      expansion
      denormalize(const expansion& xpn)
      {
        const auto& x = xpn->as<ExpansionSet>();
        return {x.valueset(), denormalize(x.valueset(), x.value())};
      }

      /// Bridge.
      template <typename ExpansionSet>
      bool
      is_normal(const expansion& xpn)
      {
        const auto& x = xpn->as<ExpansionSet>();
        return x.valueset().is_normal(x.value());
      }

      /// Bridge (normalize).
      template <typename ExpansionSet>
      expansion
      normalize_expansion(const expansion& xpn)
      {
        const auto& x = xpn->as<ExpansionSet>();
        return {x.valueset(), normalize(x.valueset(), x.value())};
      }
    }
  }
} // vcsn::
