#pragma once

#include <iosfwd>

#include <vcsn/algos/dot.hh>
#include <vcsn/algos/efsm.hh>
#include <vcsn/algos/grail.hh>
#include <vcsn/algos/info.hh>
#include <vcsn/algos/tikz.hh>
#include <vcsn/core/rat/dot.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>
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
    template <Automaton Aut>
    using has_fado_t
    = std::integral_constant<bool,
                             ((context_t_of<Aut>::is_lal
                               || context_t_of<Aut>::is_lan)
                              && std::is_same<weightset_t_of<Aut>, b>::value)>;

    template <Automaton Aut>
    auto
    fado_impl_(const Aut& aut, std::ostream& out)
      -> std::enable_if_t<has_fado_t<Aut>{}, void>
    {
      fado(aut, out);
    }

    template <Automaton Aut>
    ATTRIBUTE_NORETURN
    auto
    fado_impl_(const Aut&, std::ostream&)
      -> std::enable_if_t<!has_fado_t<Aut>{}, void>
    {
      raise("print: FAdo requires letter or nullable labels,"
            " and Boolean weights");
    }

    template <Automaton Aut>
    auto
    grail_impl_(const Aut& aut, std::ostream& out)
      -> std::enable_if_t<has_fado_t<Aut>{}, void>
    {
      grail(aut, out);
    }

    template <Automaton Aut>
    ATTRIBUTE_NORETURN
    auto
    grail_impl_(const Aut&, std::ostream&)
      -> std::enable_if_t<!has_fado_t<Aut>{}, void>
    {
      raise("print: Grail requires letter or nullable labels,"
            " and Boolean weights");
    }
  }

  template <Automaton Aut>
  std::ostream&
  print(const Aut& aut,
        std::ostream& out = std::cout, const std::string& fmt = "default")
  {
    static const auto map
      = getarg<std::function<void(const Aut&, std::ostream&)>>
      {
        "automaton output format",
        {
          {"debug",        [](const Aut& a, std::ostream& o){ a->print(o); }},
          {"dot",          [](const Aut& a, std::ostream& o){ dot(a, o); }},
          {"default",      "dot"},
          {"dot,latex",    [](const Aut& a, std::ostream& o)
                              { dot(a, o, format("latex")); }},
          {"dot,mathjax",  [](const Aut& a, std::ostream& o)
                              { dot(a, o, format("latex"), true); }},
          {"dot,utf8",     [](const Aut& a, std::ostream& o)
                              { dot(a, o, format("utf8")); }},
          {"efsm",         [](const Aut& a, std::ostream& o){ efsm(a, o); }},
          {"fado",         detail::fado_impl_<Aut>},
          {"grail",        detail::grail_impl_<Aut>},
          {"info",         [](const Aut& a, std::ostream& o){ info(a, o); }},
          {"info,detailed",[](const Aut& a, std::ostream& o){ info(a, o, true); }},
          {"null",         [](const Aut&, std::ostream&){}},
          {"tikz",         [](const Aut& a, std::ostream& o){ tikz(a, o); }},
        }
      };
    map[fmt](aut, out);
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Ostream, typename String>
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

  /// Print a value thanks to a valueset.
  ///
  /// Applies for expansions, expressions, labels, polynomials, and weights.
  template <typename ValueSet>
  std::ostream&
  print(const ValueSet& vs, const typename ValueSet::value_t& v,
        std::ostream& o = std::cout, format fmt = {})
  {
    return vs.print(v, o, format(fmt));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (print).
      template <typename ExpansionSet, typename Ostream, typename String>
      std::ostream& print_expansion(const expansion& expansion,
                                    std::ostream& o,
                                    const std::string& fmt)
      {
        const auto& e = expansion->as<ExpansionSet>();
        return vcsn::print(e.valueset(), e.value(), o, format(fmt));
      }
    }
  }

  /*-----------------------------.
  | print(expression, stream).   |
  `-----------------------------*/

  /// Print an expression.
  template <typename Context>
  std::ostream&
  print(const expressionset<Context>& rs,
        const typename expressionset<Context>::value_t& r,
        std::ostream& o = std::cout,
        const std::string& fmt = "default")
  {
    if (fmt == "dot" || fmt == "dot,logical")
      {
        auto print = make_dot_printer(rs, o);
        return print(r);
      }
    else if (fmt == "dot,physical")
      {
        auto print = make_dot_printer(rs, o, true);
        return print(r);
      }
    else
      return print(rs, r, o, format(fmt));
  }

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
        return vcsn::print(e.valueset(), e.value(), o, fmt);
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
        return vcsn::print(l.valueset(), l.value(), o, format(fmt));
      }
    }
  }

  /*---------------------------.
  | list(polynomial, stream).  |
  `---------------------------*/

  template <typename PolynomialSet>
  std::ostream&
  list(const PolynomialSet& ps, const typename PolynomialSet::value_t& p,
       std::ostream& o = std::cout)
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
        return vcsn::list(p.valueset(), p.value(), o);
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
        return vcsn::print(p.valueset(), p.value(), o, format(fmt));
      }
    }
  }


  /*------------------------.
  | print(weight, stream).  |
  `------------------------*/

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
        return vcsn::print(w.valueset(), w.value(), o, format(fmt));
      }
    }
  }
}
