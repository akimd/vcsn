#ifndef VCSN_ALGOS_SPLIT_HH
# define VCSN_ALGOS_SPLIT_HH

# include <vcsn/core/rat/visitor.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  namespace rat
  {
    // FIXME: this is a general feature which is useful elsewhere.
    // E.g., expand.

    /// Type of PolynomialSet of RatExps from the RatExpSet type.
    template <typename RatExpSet>
    using ratexp_polynomialset_t
      = polynomialset<context<RatExpSet,
                              weightset_t_of<RatExpSet>>>;

    /// Type of polynomials of ratexps from the RatExpSet type.
    template <typename RatExpSet>
    using ratexp_polynomial_t
      = typename ratexp_polynomialset_t<RatExpSet>::value_t;

    /// From a RatExpSet to its polynomialset.
    template <typename RatExpSet>
    inline
    ratexp_polynomialset_t<RatExpSet>
    make_ratexp_polynomialset(const RatExpSet& rs)
    {
      using context_t = context<RatExpSet,
                                weightset_t_of<RatExpSet>>;
      return context_t{rs, *rs.weightset()};
    }
  }


  /*----------------.
  | split(ratexp).  |
  `----------------*/

  namespace rat
  {
    /// Break a rational expression into a polynomial.
    ///
    /// This is based on the following paper:
    /// @verbatim
    /// @article{angrand.2010.jalc,
    ///   author =       {Pierre-Yves Angrand and Sylvain Lombardy and Jacques
    ///                   Sakarovitch},
    ///   journal =      {Journal of Automata, Languages and Combinatorics},
    ///   number =       {1/2},
    ///   pages =        {27--51},
    ///   title =        {On the Number of Broken Derived Terms of a Rational
    ///                   Expression},
    ///   volume =       15,
    ///   year =         2010,
    ///   abstract =     {Bounds are given on the number of broken derived
    ///                   terms (a variant of Antimirov's ``partial
    ///                   derivatives'') of a rational expression $E$. It is
    ///                   shown that this number is less than or equal to
    ///                   $2l(E) + 1$ in the general case, where $l(E)$ is the
    ///                   literal length of the expression $E$, and that the
    ///                   classical bound $l(E) + 1$ which holds for partial
    ///                   derivatives also holds for broken derived terms if E
    ///                   is in star normal form.\\In a second part of the
    ///                   paper, the influence of the bracketing of an
    ///                   expression on the number of its derived terms is
    ///                   also discussed.}
    /// }
    ///@endverbatim
    template <typename RatExpSet>
    class split_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = context_t_of<ratexpset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using ratexp_t = typename ratexpset_t::value_t;
      using weightset_t = weightset_t_of<ratexpset_t>;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_type = typename ratexpset_t::const_visitor;

      constexpr static const char* me() { return "split"; }

      split_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      /// Break a ratexp into a polynomial.
      polynomial_t operator()(const ratexp_t& v)
      {
        return split(v);
      }

      /// Easy recursion.
      polynomial_t split(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = ps_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = polynomial_t{{rs_.one(), ws_.one()}};
      }

      VCSN_RAT_VISIT(atom, e)
      {
        res_ = polynomial_t{{rs_.atom(e.value()), ws_.one()}};
      }

      VCSN_RAT_VISIT(sum, e)
      {
        polynomial_t res = ps_.zero();
        for (const auto& v: e)
          {
            v->accept(*this);
            ps_.add_here(res, res_);
          }
        res_ = std::move(res);
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns split(l) x split(r).
      /// FIXME: This is inefficient, we split the lhs way too often.
      polynomial_t product(const ratexp_t& l, const ratexp_t& r)
      {
        // B(l).
        polynomial_t l_split = split(l);
        // constant-term(B(l)).
        weight_t l_split_const = ps_.get_weight(l_split, rs_.one());
        // proper(B(l)).
        ps_.del_weight(l_split, rs_.one());

        // res = proper(B(l)).r + constant-term(B(l))B(r).
        return ps_.add(ps_.rmul(l_split, r),
                       ps_.lmul(l_split_const, split(r)));
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns l x split(r).
      polynomial_t product(const polynomial_t& l, const ratexp_t& r)
      {
        polynomial_t res;
        for (const auto& m: l)
          ps_.add_here(res, ps_.lmul(m.second, product(m.first, r)));
        return res;
      }

      /// Handle an n-ary product.
      VCSN_RAT_VISIT(prod, e)
      {
        auto res = product(e[0], e[1]);
        for (unsigned i = 2, n = e.size(); i < n; ++i)
          res = product(res, e[i]);
        res_ = std::move(res);
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns split(l) x split(r).
      /// FIXME: This is inefficient, we split the lhs way too often.
      polynomial_t conjunction(const ratexp_t& l, const ratexp_t& r)
      {
        // B(l).
        polynomial_t l_split = split(l);
        // constant-term(B(l)).
        weight_t l_split_const = ps_.get_weight(l_split, rs_.one());
        // proper(B(l)).
        ps_.del_weight(l_split, rs_.one());

        // res = proper(B(l))&r.
        polynomial_t res;
        for (const auto& e: l_split)
          ps_.add_here(res, rs_.conjunction(e.first, r), e.second);
        // res += constant-term(B(l))B(r)
        ps_.add_here(res,
                       ps_.lmul(l_split_const, split(r)));
        return res;
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns l x split(r).
      polynomial_t conjunction(const polynomial_t& l, const ratexp_t& r)
      {
        polynomial_t res;
        for (const auto& m: l)
          ps_.add_here(res, ps_.lmul(m.second, conjunction(m.first, r)));
        return res;
      }

      /// Handle an n-ary conjunction.
      VCSN_RAT_VISIT(conjunction, e)
      {
        auto res = conjunction(e[0], e[1]);
        for (unsigned i = 2, n = e.size(); i < n; ++i)
          res = conjunction(res, e[i]);
        res_ = std::move(res);
      }

      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)

      VCSN_RAT_VISIT(star, e)
      {
        res_ = polynomial_t{{e.shared_from_this(), ws_.one()}};
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.lmul(e.weight(), res_);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.rmul(res_, e.weight());
      }

    private:
      ratexpset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
      /// The result.
      polynomial_t res_;
    };
  }

  /// Split a ratexp.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  split(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e)
  {
    rat::split_visitor<RatExpSet> split{rs};
    return split(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      polynomial
      split(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.ratexpset();
        auto ps = vcsn::rat::make_ratexp_polynomialset(rs);
        return make_polynomial(ps,
                               vcsn::split<RatExpSet>(rs, e.ratexp()));
      }

      REGISTER_DECLARE(split,
                       (const ratexp& e) -> polynomial);
    }
  }

  /// Split a polynomial of ratexps.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  split(const RatExpSet& rs, const rat::ratexp_polynomial_t<RatExpSet>& p)
  {
    auto ps = rat::make_ratexp_polynomialset(rs);
    using polynomial_t = rat::ratexp_polynomial_t<RatExpSet>;
    rat::split_visitor<RatExpSet> split{rs};
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res, ps.lmul(m.second, split(m.first)));
    return res;
  }

  /// Split a polynomial of ratexps.
  template <typename PolynomialSet>
  inline
  typename PolynomialSet::value_t
  split_polynomial(const PolynomialSet& ps,
                   const typename PolynomialSet::value_t& p)
  {
    using polynomial_t = typename PolynomialSet::value_t;
    // This is a polynomial of rational expressions.
    const auto& rs = *ps.labelset();
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res, ps.lmul(m.second, split(rs, m.first)));
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSet>
      polynomial
      split_polynomial(const polynomial& poly)
      {
        const auto& p = poly->as<PolynomialSet>();
        const auto& ps = p.polynomialset();
        return make_polynomial
          (ps,
           vcsn::split_polynomial<PolynomialSet>(ps, p.polynomial()));
      }

      REGISTER_DECLARE(split_polynomial,
                       (const polynomial& p) -> polynomial);
    }
  }


} // vcsn::

#endif // !VCSN_ALGOS_SPLIT_HH
