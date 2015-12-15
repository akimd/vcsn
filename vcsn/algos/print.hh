#pragma once

#include <iosfwd>

#include <vcsn/algos/dot.hh>
#include <vcsn/algos/efsm.hh>
#include <vcsn/algos/grail.hh>
#include <vcsn/algos/info.hh>
#include <vcsn/algos/tikz.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expansion.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /*----------------------------.
  | print(automaton, stream).   |
  `----------------------------*/

  namespace detail
  {
    // FIXME: C++14 and templated variables.
    template <typename Aut>
    using has_fado_t
    = std::integral_constant<bool,
                             ((context_t_of<Aut>::is_lal
                               || context_t_of<Aut>::is_lan)
                              && std::is_same<weightset_t_of<Aut>, b>::value)>;

    template <typename Aut>
    auto
    fado_impl_(const Aut& aut, std::ostream& out)
      -> enable_if_t<has_fado_t<Aut>{}, void>
    {
      fado(aut, out);
    }

    template <typename Aut>
    ATTRIBUTE_NORETURN
    auto
    fado_impl_(const Aut&, std::ostream&)
      -> enable_if_t<!has_fado_t<Aut>{}, void>
    {
      raise("print: FAdo requires letter or nullable labels,"
            " and Boolean weights");
    }

    template <typename Aut>
    auto
    grail_impl_(const Aut& aut, std::ostream& out)
      -> enable_if_t<has_fado_t<Aut>{}, void>
    {
      grail(aut, out);
    }

    template <typename Aut>
    ATTRIBUTE_NORETURN
    auto
    grail_impl_(const Aut&, std::ostream&)
      -> enable_if_t<!has_fado_t<Aut>{}, void>
    {
      raise("print: Grail requires letter or nullable labels,"
            " and Boolean weights");
    }
  }

  template <typename Aut>
  std::ostream&
  print(const Aut& aut, std::ostream& out, const std::string& fmt)
  {
    static const auto map
      = std::map<std::string, std::function<void(const Aut&, std::ostream&)>>
      {
        {"dot",          [](const Aut& a, std::ostream& o){ dot(a, o); }},
        {"default",      [](const Aut& a, std::ostream& o){ dot(a, o); }},
        {"dot,latex",    [](const Aut& a, std::ostream& o)
                            { dot(a, o, format("latex")); }},
        {"dot,utf8",     [](const Aut& a, std::ostream& o)
                            { dot(a, o, format("utf8")); }},
        {"efsm",         [](const Aut& a, std::ostream& o){ efsm(a, o); }},
        {"fado",         detail::fado_impl_<Aut>},
        {"grail",        detail::grail_impl_<Aut>},
        {"info",         [](const Aut& a, std::ostream& o){ info(a, o); }},
        {"info,detailed",[](const Aut& a, std::ostream& o){ info(a, o, true); }},
        {"null",         [](const Aut&, std::ostream&){}},
        {"tikz",         [](const Aut& a, std::ostream& o){ tikz(a, o); }},
      };
    auto fun = getargs("automaton output format", map, fmt);
    fun(aut, out);
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream, typename String>
      std::ostream& print(const automaton& aut, std::ostream& o,
                          const std::string& fmt)
      {
        const auto& a = aut->as<Aut>();
        return print(a, o, fmt);
      }
    }
  }

  /*-------------------------.
  | print(context, stream).  |
  `-------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (print).
      template <typename Context, typename Ostream, typename String>
      std::ostream& print_context(const context& ctx, std::ostream& o,
                                  const std::string& fmt)
      {
        const auto& c = ctx->as<Context>();
        return c.print_set(o, format(fmt));
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
        std::ostream& o, format fmt)
  {
    return vs.print(v, o, format(fmt));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (print).
      template <typename ExpansionSet, typename Ostream, typename String>
      std::ostream& print_expansion(const expansion& expansion, std::ostream& o,
                                    const std::string& fmt)
      {
        const auto& e = expansion->as<ExpansionSet>();
        return vcsn::print(e.expansionset(), e.expansion(), o, format(fmt));
      }
    }
  }

  /*-----------------------.
  | print(label, stream).  |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (print).
      template <typename LabelSet, typename Ostream, typename String>
      std::ostream& print_label(const label& label, std::ostream& o,
                                const std::string& fmt)
      {
        const auto& l = label->as<LabelSet>();
        return vcsn::print(l.labelset(), l.label(), o, format(fmt));
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
      /// Bridge (list).
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
      /// Bridge (print).
      template <typename PolynomialSet, typename Ostream, typename String>
      std::ostream& print_polynomial(const polynomial& polynomial,
                                     std::ostream& o, const std::string& fmt)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::print(p.polynomialset(), p.polynomial(), o, format(fmt));
      }
    }
  }


  /*-----------------------------.
  | print(expression, stream).   |
  `-----------------------------*/

#if 0
  /// See PolynomialSet.
  template <typename ExpSet>
  inline
  std::ostream&
  print(const ExpSet& rs, const typename ExpSet::value_t& e,
        std::ostream& o, format fmt)
  {
    return rs.print(e, o, format(fmt));
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (print).
      template <typename ExpSet, typename Ostream, typename String>
      std::ostream& print_expression(const expression& exp, std::ostream& o,
                                     const std::string& fmt)
      {
        const auto& e = exp->as<ExpSet>();
        return vcsn::print(e.expressionset(), e.expression(), o, format(fmt));
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
      /// Bridge (print).
      template <typename WeightSet, typename Ostream, typename String>
      std::ostream& print_weight(const weight& weight, std::ostream& o,
                                 const std::string& fmt)
      {
        const auto& w = weight->as<WeightSet>();
        return vcsn::print(w.weightset(), w.weight(), o, format(fmt));
      }
    }
  }
}
