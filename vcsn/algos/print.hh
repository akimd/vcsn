#pragma once

#include <iosfwd>

#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expansion.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*-------------------------.
  | print(context, stream).  |
  `-------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Ostream, typename String>
      std::ostream& print_ctx(const context& ctx, std::ostream& o,
                              const std::string& format)
      {
        const auto& c = ctx->as<Context>();
        return c.print_set(o, format);
      }
    }
  }

  /*---------------------------.
  | print(expansion, stream).  |
  `---------------------------*/

  /// Applies to (ValueSet, Value, ostream, string): for expansionset,
  /// polynomialset, expressionset, and weightset.
  template <typename ValueSet>
  inline
  std::ostream&
  print(const ValueSet& vs, const typename ValueSet::value_t& v,
        std::ostream& o, const std::string& format)
  {
    return vs.print(v, o, format);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpansionSet, typename Ostream, typename String>
      std::ostream& print_expansion(const expansion& expansion, std::ostream& o,
                                    const std::string& format)
      {
        const auto& e = expansion->as<ExpansionSet>();
        return vcsn::print(e.expansionset(), e.expansion(), o, format);
      }
    }
  }

  /*-----------------------.
  | print(stream, label).  |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename LabelSet, typename Ostream, typename String>
      std::ostream& print_label(const label& label, std::ostream& o,
                                const std::string& format)
      {
        const auto& l = label->as<LabelSet>();
        return vcsn::print(l.labelset(), l.label(), o, format);
      }
    }
  }

  /*---------------------------.
  | list(polynomial, stream).  |
  `---------------------------*/

  template <typename PolynomialSet>
  inline
  std::ostream&
  list(const PolynomialSet& ps, const typename PolynomialSet::value_t& p,
       std::ostream& o)
  {
    bool first = true;
    for (const auto& m: p)
      {
        if (!first)
          o << std::endl;
        first = false;
        ps.print(m, o);
      }
    return o;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSet, typename Ostream>
      std::ostream& list_polynomial(const polynomial& polynomial,
                                    std::ostream& o)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::list(p.polynomialset(), p.polynomial(), o);
      }
    }
  }

  /*----------------------------.
  | print(polynomial, stream).  |
  `----------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSet, typename Ostream, typename String>
      std::ostream& print_polynomial(const polynomial& polynomial,
                                     std::ostream& o, const std::string& format)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::print(p.polynomialset(), p.polynomial(), o, format);
      }
    }
  }


  /*------------------------.
  | print(expression, stream).  |
  `------------------------*/

#if 0
  /// See PolynomialSet.
  template <typename ExpSet>
  inline
  std::ostream&
  print(const ExpSet& rs, const typename ExpSet::value_t& e,
        std::ostream& o, const std::string& format)
  {
    return rs.print(e, o, format);
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet, typename Ostream, typename String>
      std::ostream& print_expression(const expression& exp, std::ostream& o,
                                     const std::string& format)
      {
        const auto& e = exp->as<ExpSet>();
        return vcsn::print(e.expressionset(), e.expression(), o, format);
      }
    }
  }

  /*------------------------.
  | print(weight, stream).  |
  `------------------------*/

#if 0
  /// See PolynomialSet.
  template <typename WeightSet>
  inline
  std::ostream&
  print(const WeightSet& ws, const typename WeightSet::value_t& w,
        std::ostream& o)
  {
    return ws.print(w, o);
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename WeightSet, typename Ostream, typename String>
      std::ostream& print_weight(const weight& weight, std::ostream& o,
                                 const std::string& format)
      {
        const auto& w = weight->as<WeightSet>();
        return vcsn::print(w.weightset(), w.weight(), o, format);
      }
    }
  }
}
