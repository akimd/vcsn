#pragma once

#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*-------------.
  | read_label.  |
  `-------------*/

  template <typename Context>
  inline
  auto
  read_label(const Context& ctx, std::istream& is,
             bool quoted = false)
    -> label_t_of<Context>
  {
    return ctx.labelset()->conv(is, quoted);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Istream, typename Bool>
      label
      read_label(const context& ctx, std::istream& is, bool quoted)
      {
        const auto& c = ctx->as<Context>();
        auto res = ::vcsn::read_label(c, is, quoted);
        return {*c.labelset(), res};
      }
    }
  }


  /*------------------.
  | read_polynomial.  |
  `------------------*/

  template <typename Context>
  inline
  auto
  read_polynomial(const Context& ctx, std::istream& is)
    -> typename polynomialset<Context>::value_t
  {
    return polynomialset<Context>(ctx).conv(is);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Istream>
      polynomial
      read_polynomial(const context& ctx, std::istream& is)
      {
        const auto& c = ctx->as<Context>();
        auto ps = polynomialset<Context>(c);
        auto res = ::vcsn::read_polynomial(c, is);
        return {ps, res};
      }
    }
  }


  /*--------------.
  | read_weight.  |
  `--------------*/

  template <typename Context>
  inline
  auto
  read_weight(const Context& ctx, std::istream& is)
    -> weight_t_of<Context>
  {
    return ctx.weightset()->conv(is);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Istream>
      weight
      read_weight(const context& ctx, std::istream& is)
      {
        const auto& c = ctx->as<Context>();
        auto res = ::vcsn::read_weight(c, is);
        return {*c.weightset(), res};
      }
    }
  }
} // namespace vcsn
