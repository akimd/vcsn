#pragma once

#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/rat/expansionset.hh>

namespace vcsn
{
  /// The projection of an expansion as an expression.
  ///
  /// \param xs    The expansionset.
  /// \param x     The expansion.
  template <typename ExpansionSet>
  typename ExpansionSet::expression_t
  to_expression(const ExpansionSet& xs,
                const typename ExpansionSet::value_t& x)
  {
    const auto& es = xs.expressionset();
    const auto& ps = xs.polynomialset();
    auto res = es.lweight(x.constant, es.one());
    for (const auto& p: x.polynomials)
      res = es.add(res,
                   es.mul(es.atom(p.first), ps.to_label(p.second)));
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (to_expression).
      template <typename ExpansionSet>
      expression
      to_expression_expansion(const expansion& exp)
      {
        const auto& x = exp->as<ExpansionSet>();
        return {x.valueset(), to_expression(x.valueset(), x.value())};
      }
    }
  }
}
