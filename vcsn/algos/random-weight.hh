#pragma once

#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /// Generate a random weight.
  template <typename WeightSet>
  typename WeightSet::value_t
  random_weight(const WeightSet& ws, const std::string& param)
  {
    auto random_w = detail::random_weight<WeightSet,
                                          std::mt19937>{make_random_engine(), ws};
    random_w.parse_param(param);
    return random_w.generate_random_weight();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename String>
      weight
      random_weight(const context& ctx, const std::string& param)
      {
        const auto& c = ctx->as<Context>();
        const auto& ws = *c.weightset();
        return {ws, random_weight(ws, param)};
      }
    }
  }
}
