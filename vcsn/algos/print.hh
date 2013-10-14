#ifndef VCSN_ALGOS_PRINT_HH
# define VCSN_ALGOS_PRINT_HH

# include <iosfwd>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/weight.hh>
# include <vcsn/core/rat/ratexpset.hh>

namespace vcsn
{

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

  template <typename PolynomialSet>
  inline
  std::ostream&
  print(const PolynomialSet& ps, const typename PolynomialSet::value_t& p,
        std::ostream& o)
  {
    return ps.print(o, p);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSet>
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
      template <typename PolynomialSet>
      std::ostream& print_polynomial(const polynomial& polynomial,
                                     std::ostream& o)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::print<PolynomialSet>(p.get_polynomialset(),
                                          p.polynomial(), o);
      }

      REGISTER_DECLARE(print_polynomial,
                       (const polynomial& p, std::ostream& o) -> std::ostream&);
    }
  }


  /*------------------------.
  | print(ratexp, stream).  |
  `------------------------*/

#if 0
  // There is no need for this implementation, as the previous one,
  // for weightset/weight, applies here.  Actually, it results in an
  // ambiguous template specialization.
  template <typename RatExpSet>
  inline
  std::ostream&
  print(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
        std::ostream& o)
  {
    return rs.print(o, e);
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      std::ostream& print_exp(const ratexp& exp, std::ostream& o)
      {
        const auto& e = exp->as<RatExpSet>();
        return vcsn::print(e.get_ratexpset(), e.ratexp(), o);
      }

      REGISTER_DECLARE(print_exp,
                       (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }

  /*------------------------.
  | print(weight, stream).  |
  `------------------------*/

#if 0
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
      template <typename WeightSet>
      std::ostream& print_weight(const weight& weight, std::ostream& o)
      {
        const auto& w = weight->as<WeightSet>();
        return vcsn::print<WeightSet>(w.get_weightset(), w.weight(), o);
      }

      REGISTER_DECLARE(print_weight,
                       (const weight& aut, std::ostream& o) -> std::ostream&);
    }
  }

}

#endif // !VCSN_ALGOS_PRINT_HH
