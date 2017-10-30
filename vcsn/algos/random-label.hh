#pragma once

#include <random> // std::mt19937

#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/labelset/labelset.hh> // detail::random_label.

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename String>
      label
      random_label(const context& ctx, const std::string& param)
      {
        const auto& c = ctx->as<Context>();
        const auto& ls = *c.labelset();
        return {ls, random_label(ls, param)};
      }
    }
  }
}
