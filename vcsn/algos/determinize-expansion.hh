#pragma once

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /// Determinize an expansion.
  template <typename ExpansionSet>
  typename ExpansionSet::value_t
  determinize(const ExpansionSet& xs, const typename ExpansionSet::value_t& x)
  {
    return xs.denormalize(x);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (determinize).
      template <typename ExpansionSet>
      expansion
      determinize_expansion(const expansion& xpn)
      {
        const auto& x = xpn->as<ExpansionSet>();
        return {x.valueset(), determinize(x.valueset(), x.value())};
      }
    }
  }
} // vcsn::
