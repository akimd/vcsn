#pragma once

#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  namespace rat
  {
    // FIXME: this is a general feature which is useful elsewhere.
    // E.g., expand.

    /// Type of PolynomialSet of expressions from the ExpSet type.
    template <typename ExpSet>
    using expression_polynomialset_t
      = polynomialset<context<ExpSet,
                              weightset_t_of<ExpSet>>>;

    /// Type of polynomials of expressions from the ExpSet type.
    template <typename ExpSet>
    using expression_polynomial_t
      = typename expression_polynomialset_t<ExpSet>::value_t;

    /// From a ExpSet to its polynomialset.
    template <typename ExpSet>
    expression_polynomialset_t<ExpSet>
    make_expression_polynomialset(const ExpSet& rs)
    {
      using context_t = context<ExpSet,
                                weightset_t_of<ExpSet>>;
      return context_t{rs, *rs.weightset()};
    }
  }


  /*---------------------.
  | split(expression).   |
  `---------------------*/

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
    template <typename ExpSet>
    class split_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using context_t = context_t_of<expressionset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_t = typename expressionset_t::const_visitor;

      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "split"; }

      split_visitor(const expressionset_t& rs)
        : rs_(rs)
      {}

      /// Break an expression into a polynomial.
      polynomial_t operator()(const expression_t& v)
      {
        return split(v);
      }

      /// Easy recursion.
      polynomial_t split(const expression_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = ps_.zero();
      }

      VCSN_RAT_VISIT(add, e)
      {
        auto res = ps_.zero();
        for (const auto& v: e)
          {
            v->accept(*this);
            ps_.add_here(res, res_);
          }
        res_ = std::move(res);
      }

      /// The split-multiplation of \a l with \a r.
      ///
      /// Returns split(l) x split(r).
      polynomial_t multiply(const expression_t& l, const expression_t& r)
      {
        // B(l).
        polynomial_t l_split = split(l);
        // constant-term(B(l)).
        weight_t l_split_const = ps_.get_weight(l_split, rs_.one());
        // proper(B(l)).
        ps_.del_weight(l_split, rs_.one());

        // res = proper(B(l)) x r.
        auto res = ps_.zero();
        for (const auto& e: l_split)
          ps_.add_here(res, rs_.mul(label_of(e), r), weight_of(e));
        // res += ⟨constant-term(B(l))⟩.B(r)
        ps_.add_here(res,
                     ps_.lweight(l_split_const, split(r)));
        return res;
      }

      /// The split-multiplation of \a l with \a r.
      ///
      /// Returns l x split(r).
      polynomial_t multiply(const polynomial_t& l, const expression_t& r)
      {
        auto res = ps_.zero();
        /// FIXME: This is inefficient, we split the lhs way too often.
        for (const auto& m: l)
          ps_.add_here(res,
                       ps_.lweight(weight_of(m),
                                   multiply(label_of(m), r)));
        return res;
      }

      /// Handle an n-ary multiplication.
      VCSN_RAT_VISIT(mul, e)
      {
        auto res = multiply(e[0], e[1]);
        for (unsigned i = 2, n = e.size(); i < n; ++i)
          res = multiply(res, e[i]);
        res_ = std::move(res);
      }

      /// These are just propagated as monomials.
#define DEFINE(Type)                                            \
      VCSN_RAT_VISIT(Type, e)                                   \
      {                                                         \
        res_ = polynomial_t{{e.shared_from_this(), ws_.one()}}; \
      }

      DEFINE(atom)
      DEFINE(complement)
      DEFINE(compose)
      DEFINE(conjunction)
      DEFINE(infiltrate)
      DEFINE(ldivide)
      DEFINE(name)
      DEFINE(one)
      DEFINE(shuffle)
      DEFINE(star)
      DEFINE(transposition)
#undef DEFINE

      using tuple_t = typename super_t::tuple_t;
      void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.lweight(e.weight(), res_);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.rweight(res_, e.weight());
      }

    private:
      expressionset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// The result.
      polynomial_t res_;
    };
  }

  /// Split an expression.
  template <typename ExpSet>
  rat::expression_polynomial_t<ExpSet>
  split(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    rat::split_visitor<ExpSet> split{rs};
    return split(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      polynomial
      split(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& rs = e.valueset();
        auto ps = vcsn::rat::make_expression_polynomialset(rs);
        return {ps, vcsn::split<ExpSet>(rs, e.value())};
      }
    }
  }

  /// Split a polynomial of expressions, given the polynomialset.
  template <typename PolynomialSet>
  typename PolynomialSet::value_t
  split_polynomial(const PolynomialSet& ps,
                   const typename PolynomialSet::value_t& p)
  {
    // This is a polynomial of rational expressions.
    const auto& rs = *ps.labelset();
    auto res = ps.zero();
    for (const auto& m: p)
      res = ps.add(res, ps.lweight(weight_of(m), split(rs, label_of(m))));
    return res;
  }

  /// Split a polynomial of expressions, given the expressionset.
  template <typename ExpSet>
  rat::expression_polynomial_t<ExpSet>
  split(const ExpSet& rs, const rat::expression_polynomial_t<ExpSet>& p)
  {
    return split_polynomial(rat::make_expression_polynomialset(rs), p);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (split).
      template <typename PolynomialSet>
      polynomial
      split_polynomial(const polynomial& poly)
      {
        const auto& p = poly->as<PolynomialSet>();
        const auto& ps = p.valueset();
        return {ps, vcsn::split_polynomial<PolynomialSet>(ps, p.value())};
      }
    }
  }
} // vcsn::
