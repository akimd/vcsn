#pragma once

#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Identities>
      expression expression_one(const context& ctx, identities ids)
      {
        const auto& c = ctx->as<Context>();
        auto rs = vcsn::make_expressionset(c, ids);
        return {rs, rs.one()};
      }

      /// Bridge.
      template <typename Context, typename Identities>
      expression expression_zero(const context& ctx, identities ids)
      {
        const auto& c = ctx->as<Context>();
        auto rs = vcsn::make_expressionset(c, ids);
        return {rs, rs.zero()};
      }

      /// Bridge.
      template <typename Context>
      weight weight_one(const context& ctx)
      {
        const auto& c = ctx->as<Context>();
        auto ws = *c.weightset();
        return {ws, ws.one()};
      }

      /// Bridge.
      template <typename Context>
      weight weight_zero(const context& ctx)
      {
        const auto& c = ctx->as<Context>();
        auto ws = *c.weightset();
        return {ws, ws.zero()};
      }
    }
  }
} // namespace vcsn
