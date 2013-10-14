#ifndef VCSN_ALGOS_DERIVE_HH
# define VCSN_ALGOS_DERIVE_HH

# include <set>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/algos/constant-term.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/weights/polynomialset.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/polynomial.hh>

namespace vcsn
{

  namespace rat
  {

    /// Type of PolynomialSet of RatExps from the RatExpSet type.
    template <typename RatExpSet>
    using ratexp_polynomialset_t
      = polynomialset<ctx::context<RatExpSet,
                                   typename RatExpSet::weightset_t>>;

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
      using context_t = ctx::context<RatExpSet,
                                     typename RatExpSet::weightset_t>;
      return context_t{rs, *rs.weightset()};
    }


    /*-----------------.
    | derive(ratexp).  |
    `-----------------*/

    template <typename RatExpSet>
    class derive_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using ratexp_t = typename ratexpset_t::ratexp_t;
      using weightset_t = typename ratexpset_t::weightset_t;
      using weight_t = typename ratexpset_t::weight_t;
      /// FIXME: use a letter_t type.
      using letter_t = char;

      using polynomialset_t = ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using nary_t = typename super_type::nary_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      derive_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      polynomial_t
      operator()(const ratexp_t& v, letter_t var)
      {
        variable_ = var;
        v->accept(*this);
        return std::move(res_);
      }


      void
      apply_weights(const inner_t& e)
      {
        res_ = ps_.lmul(e.left_weight(), res_);
        res_ = ps_.rmul(res_, e.right_weight());
      }

      void
      apply_weights(const leaf_t& e)
      {
        res_ = ps_.lmul(e.left_weight(), res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = ps_.zero();
      }

      virtual void
      visit(const one_t&)
      {
        res_ = ps_.zero();
      }

      virtual void
      visit(const atom_t& e)
      {
        if (e.value() == variable_)
          {
            res_ = ps_.one();
            apply_weights(e);
          }
        else
          res_ = ps_.zero();
      }

      virtual void
      visit(const sum_t& e)
      {
        polynomial_t res = ps_.zero();
        for (auto v: e)
          {
            v->accept(*this);
            res = ps_.add(res, res_);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const prod_t& e)
      {
        // We generate a sum.
        auto res = ps_.zero();
        // Accumulate the product of the constant terms of the
        // previous factors.
        weight_t constant = ws_.one();
        for (unsigned i = 0, n = e.size(); i < n; ++i)
          {
            const auto& v = e[i];
            v->accept(*this);
            for (unsigned j = i + 1; j < n; ++j)
              res_ = ps_.rmul(res_, e[j]);
            res = ps_.add(res, ps_.lmul(constant, res_));
            constant = ws_.mul(constant, constant_term(rs_, v));
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        // We need a copy of e, but without its weights.
        auto e2 = rs_.star(e.sub()->clone());
        res_ = ps_.lmul(ws_.star(constant_term(rs_, e.sub())),
                        ps_.rmul(res_, e2));
        apply_weights(e);
      }

    private:
      ratexpset_t rs_;
      weightset_t ws_ = *rs_.weightset();
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
      polynomial_t res_;
      /// The derivation variable.
      letter_t variable_;
    };

  } // rat::

  /// Derive a ratexp wrt to a letter.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derive(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
         char a)
  {
    rat::derive_visitor<RatExpSet> derive{rs};
    return derive(e, a);
  }


  /// Derive a polynonials of ratexp wrt to a letter.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derive(const RatExpSet& rs,
         const rat::ratexp_polynomial_t<RatExpSet>& p,
         char a)
  {
    auto ps = rat::make_ratexp_polynomialset(rs);
    using polynomial_t = rat::ratexp_polynomial_t<RatExpSet>;
    rat::derive_visitor<RatExpSet> derive{rs};
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res, ps.lmul(m.second, derive(m.first, a)));
    return res;
  }


  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derive(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
         const std::string& s)
  {
    if (s.empty())
      throw std::runtime_error("cannot derive wrt an empty string");
    auto res = derive(rs, e, s[0]);
    for (size_t i = 1, len = s.size(); i < len; ++i)
      res = derive(rs, res, s[i]);
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /*---------------------------.
      | dyn::derive(exp, string).  |
      `---------------------------*/
      template <typename RatExpSet>
      polynomial
      derive(const ratexp& exp, const std::string& s)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.get_ratexpset();
        auto ps = vcsn::rat::make_ratexp_polynomialset(rs);
        return make_polynomial(ps,
                               derive<RatExpSet>(rs, e.ratexp(), s));
      }

      REGISTER_DECLARE(derive,
                       (const ratexp& e, const std::string& s) -> polynomial);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_DERIVE_HH
