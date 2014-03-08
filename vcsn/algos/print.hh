#ifndef VCSN_ALGOS_PRINT_HH
# define VCSN_ALGOS_PRINT_HH

# include <iosfwd>

# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/weight.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  template <typename LabelSet, typename WeightSet>
  inline
  std::ostream&
  print_ctx(const ctx::context<LabelSet, WeightSet>& ctx,
            std::ostream& o, const std::string& format)
  {
    print_set(*ctx.labelset(), o, format);
    if (format == "latex")
      o << "\\rightarrow";
    else
      o << '_';
    return print_set(*ctx.weightset(), o, format);
  }

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
        return vcsn::print_ctx<typename Context::labelset_t,
                               typename Context::weightset_t>(c, o, format);
      }

      REGISTER_DECLARE(print_ctx,
                       (const context& c, std::ostream& o,
                        const std::string& format) -> std::ostream&);
    }
  }

  /*---------------------------------.
  | list/print(polynomial, stream).  |
  `---------------------------------*/

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
        ps.print(o, m);
      }
    return o;
  }

  /// Actually applies to (ValueSet, Value, ostream, string): for
  /// polynomialset, ratexpset, and weightset.
  template <typename PolynomialSet>
  inline
  std::ostream&
  print(const PolynomialSet& ps, const typename PolynomialSet::value_t& p,
        std::ostream& o, const std::string& format)
  {
    return ps.print(o, p, format);
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
        return vcsn::list<PolynomialSet>(p.get_polynomialset(),
                                         p.polynomial(), o);
      }

      REGISTER_DECLARE(list_polynomial,
                       (const polynomial& p, std::ostream& o) -> std::ostream&);

      /// Bridge.
      template <typename PolynomialSet, typename Ostream, typename String>
      std::ostream& print_polynomial(const polynomial& polynomial,
                                     std::ostream& o, const std::string& format)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::print<PolynomialSet>(p.get_polynomialset(),
                                          p.polynomial(), o, format);
      }

      REGISTER_DECLARE(print_polynomial,
                       (const polynomial& p, std::ostream& o,
                        const std::string& format) -> std::ostream&);
    }
  }


  /*------------------------.
  | print(ratexp, stream).  |
  `------------------------*/

#if 0
  /// See PolynomialSet.
  template <typename RatExpSet>
  inline
  std::ostream&
  print(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
        std::ostream& o, const std::string& format)
  {
    return rs.print(o, e, format);
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Ostream, typename String>
      std::ostream& print_ratexp(const ratexp& exp, std::ostream& o,
                                 const std::string& format)
      {
        const auto& e = exp->as<RatExpSet>();
        return vcsn::print(e.get_ratexpset(), e.ratexp(), o, format);
      }

      REGISTER_DECLARE(print_ratexp,
                       (const ratexp& aut, std::ostream& o,
			const std::string& format) -> std::ostream&);
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
    return ws.print(o, w);
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
        return vcsn::print<WeightSet>(w.get_weightset(), w.weight(), o, format);
      }

      REGISTER_DECLARE(print_weight,
                       (const weight& aut, std::ostream& o,
                        const std::string& format) -> std::ostream&);
    }
  }

}

#endif // !VCSN_ALGOS_PRINT_HH
