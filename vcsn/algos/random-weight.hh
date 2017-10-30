#pragma once

#include <random> // std::mt19937

#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/weightset/weightset.hh> // detail::random_weight.

namespace vcsn
{
  /// Generate a random weight.
  template <typename WeightSet,
            typename RandomGenerator = std::mt19937>
  typename WeightSet::value_t
  random_weight(const WeightSet& ws, const std::string& param = {},
                RandomGenerator& gen = make_random_engine())
  {
    auto rand = detail::random_weight<WeightSet, RandomGenerator>{ws, gen};
    rand.parse_param(param);
    return rand();
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
