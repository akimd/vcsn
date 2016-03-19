#pragma once

#include <vcsn/ctx/project-context.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/name.hh> // integral_constant
#include <vcsn/dyn/label.hh>

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
        return vcsn::detail::project<Tape::value>(c);
      }
    }
  }


  /*--------------------------.
  | project(expressionset).   |
  `--------------------------*/

  namespace detail
  {
    /// Project an expressionset to one tape.
    template <size_t Tape, typename Context>
    auto project(const expressionset<Context>& rs)
    {
      // Weirdly enough, GCC enters an endless loop of instantiation
      // if we use this type as explicit function return type.
      return make_expressionset(project<Tape>(rs.context()), rs.identities());
    }
  }

  /*-----------------------.
  | project(polynomial).   |
  `-----------------------*/

  namespace detail
  {
    /// Project a value to one tape.
    template <size_t Tape, typename ValueSet>
    auto project(const ValueSet& vs,
                 const typename ValueSet::value_t& v)
    {
      static_assert(Tape < labelset_t_of<ValueSet>::size(),
                    "project: invalid tape number");
      return vs.template project<Tape>(v);
    }

    /// Project a polynomialset to one tape.
    template <size_t Tape, typename Context, wet_kind_t Kind>
    auto project(const polynomialset<Context, Kind>& ps)
    {
      static_assert(Tape < labelset_t_of<Context>::size(),
                    "project: invalid tape number");
      auto ctx_out = vcsn::detail::project<Tape>(ps.context());
      return make_polynomialset(ctx_out);
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
        constexpr auto tape = Tape::value;
        auto& p = poly->as<PolynomialSet>();
        const auto& ps_in = p.polynomialset();
        auto ps_out = vcsn::detail::project<tape>(ps_in);
        return make_polynomial(ps_out,
                               vcsn::detail::project<tape>(ps_in,
                                                           p.polynomial()));
      }
    }
  }

  /*------------------.
  | project(label).   |
  `------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (project).
      template <typename Label, typename Tape>
      label
      project_label(const label& lbl, integral_constant)
      {
        constexpr auto tape = Tape::value;
        const auto& l = lbl->as<Label>();
        const auto& ls = l.labelset();
        return {ls.template set<tape>(), std::get<tape>(l.label())};
      }
    }
  }
}
