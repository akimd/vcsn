#pragma once

#include <vcsn/algos/tuple.hh>
#include <vcsn/core/rat/copy.hh>
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
        return {rs, copy(e.valueset(), rs, e.value())};
      }
    }
  }
}
