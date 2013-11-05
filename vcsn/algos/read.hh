#ifndef VCSN_ALGOS_READ_HH
# define VCSN_ALGOS_READ_HH

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/weights/polynomialset.hh>
# include <vcsn/dyn/weight.hh>

namespace vcsn
{

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
      template <typename Context>
      auto
      read_polynomial(const context& ctx, std::istream& is)
        -> polynomial
      {
        const auto& c = ctx->as<Context>();
        auto ps = polynomialset<Context>(c);
        auto res = ::vcsn::read_polynomial(c, is);
        return make_polynomial(ps, res);
      }

      REGISTER_DECLARE(read_polynomial,
                       (const context& ctx, std::istream& is) -> polynomial);
    }
  }


  /*--------------.
  | read_weight.  |
  `--------------*/

  template <typename Context>
  inline
  auto
  read_weight(const Context& ctx, std::istream& is)
    -> typename Context::weight_t
  {
    return ctx.weightset()->conv(is);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context>
      auto
      read_weight(const context& ctx, std::istream& is)
        -> weight
      {
        const auto& c = ctx->as<Context>();
        auto res = ::vcsn::read_weight(c, is);
        return make_weight(*c.weightset(), res);
      }

      REGISTER_DECLARE(read_weight,
                       (const context& ctx, std::istream& is) -> weight);
    }
  }


} // namespace vcsn

#endif // !VCSN_ALGOS_READ_HH
