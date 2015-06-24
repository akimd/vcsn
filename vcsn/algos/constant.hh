#pragma once

#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expression.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Identities>
      expression expression_one(const context& ctx, rat::identities ids)
      {
        const auto& c = ctx->as<Context>();
        auto rs = vcsn::make_expressionset(c, ids);
        return dyn::make_expression(rs, rs.one());
      }

      /// Bridge.
      template <typename Context, typename Identities>
      expression expression_zero(const context& ctx, rat::identities ids)
      {
        const auto& c = ctx->as<Context>();
        auto rs = vcsn::make_expressionset(c, ids);
        return dyn::make_expression(rs, rs.zero());
      }
    }
  }
} // namespace vcsn
