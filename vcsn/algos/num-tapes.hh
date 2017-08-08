#pragma once

#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/context.hh>

namespace vcsn
{
  /*-------------.
  | num_tapes.   |
  `-------------*/

  template <typename Ctx>
  constexpr auto
  num_tapes()
    -> size_t
  {
    return number_of_tapes<Ctx>::value;
  }

  template <typename Ctx>
  constexpr auto
  num_tapes(const Ctx&)
    -> size_t
  {
    return num_tapes<Ctx>();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx>
      size_t
      num_tapes(const context& ctx)
      {
        return vcsn::num_tapes(ctx->as<Ctx>());
      }
    }
  }
}
