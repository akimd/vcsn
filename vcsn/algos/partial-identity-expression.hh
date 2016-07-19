#pragma once

#include <vcsn/algos/tuple.hh>
#include <vcsn/core/rat/partial-identity.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*--------------------------------.
  | partial_identity(expression).   |
  `--------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (partial_identity).
      template <typename ExpressionSet>
      expression
      partial_identity_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpressionSet>();
        auto rs = tuple_expressionset(e.valueset(), e.valueset());
        return {rs, partial_identity(e.valueset(), rs, e.value())};
      }
    }
  }
}
