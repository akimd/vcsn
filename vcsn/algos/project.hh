#pragma once

#include <vcsn/ctx/project-context.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/name.hh> // integral_constant

namespace vcsn
{
  /*--------------------.
  | project(context).   |
  `--------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (project).
      template <typename Context, typename Tape>
      context
      project_context(const context& ctx, integral_constant)
      {
        auto& c = ctx->as<Context>();
        return make_context(vcsn::detail::make_project_context<Tape::value>(c));
      }
    }
  }


  /*--------------------------.
  | project(expressionset).   |
  `--------------------------*/

  namespace detail
  {
    template <size_t Tape, typename Context>
    auto make_project(const expressionset<Context>& rs)
      -> expressionset<decltype(make_project_context<Tape>(rs.context()))>
    {
      return {make_project_context<Tape>(rs.context()),
              rs.identities()};
    }
  }

  /*-----------------------.
  | project(polynomial).   |
  `-----------------------*/

  namespace detail
  {
    template <size_t Tape, typename ValueSet>
    auto project(const ValueSet& vs,
                 const typename ValueSet::value_t& v)
    {
      static_assert(Tape < labelset_t_of<ValueSet>::size(),
                    "project: invalid tape number");
      return vs.template project<Tape>(v);
    }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (project).
      template <typename PolynomialSet, typename Tape>
      polynomial
      project_polynomial(const polynomial& poly, integral_constant)
      {
        auto& p = poly->as<PolynomialSet>();
        const auto& ps_in = p.polynomialset();
        auto ctx_out
          = vcsn::detail::make_project_context<Tape::value>(ps_in.context());
        auto ps_out = make_polynomialset(ctx_out);
        return make_polynomial
          (ps_out,
           vcsn::detail::project<Tape::value>(ps_in, p.polynomial()));
      }
    }
  }
}
