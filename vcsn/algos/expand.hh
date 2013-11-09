#ifndef VCSN_ALGOS_EXPAND_HH
# define VCSN_ALGOS_EXPAND_HH

# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>

# include <vcsn/algos/derivation.hh> // ratexp_polynomialset_t.

namespace vcsn
{

  namespace rat
  {

    /*-----------------.
    | expand(ratexp).  |
    `-----------------*/

    /// \tparam RatExpSet  relative to the RatExp.
    template <typename RatExpSet>
    class expand_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using ratexp_t = typename ratexpset_t::value_t;
      using context_t = typename ratexpset_t::context_t;
      using weightset_t = typename ratexpset_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_type = typename RatExpSet::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using prod_t = typename super_type::prod_t;
      using intersection_t = typename super_type::intersection_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      expand_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      ratexp_t
      operator()(const ratexp_t& v)
      {
        v->accept(*this);
        return ratexp(res_);
      }

      ratexp_t
      ratexp(const polynomial_t p)
      {
        ratexp_t res = rs_.zero();
        for (const auto& m: p)
          res = rs_.add(res, rs_.weight(m.second, m.first));
         return res;
      }

      /// Syntactic sugar: recursive call to this visitor.
      polynomial_t expand(const ratexp_t& e)
      {
        e->accept(*this);
        return res_;
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = ps_.zero();
      }

      virtual void
      visit(const one_t& v)
      {
        res_ = polynomial_t{{rs_.one(), v.left_weight()}};
      }

      virtual void
      visit(const atom_t& v)
      {
        res_ = polynomial_t{{rs_.atom(v.value()), v.left_weight()}};
      }

      virtual void
      visit(const sum_t& v)
      {
        polynomial_t res = ps_.zero();
        for (auto c: v)
          res = ps_.add(res, expand(c));
        res_ = ps_.rmul(ps_.lmul(v.left_weight(), res), v.right_weight());
      }

      virtual void
      visit(const intersection_t& v)
      {
        auto res = expand(v.head());
        for (auto c: v.tail())
          {
            polynomial_t sum = ps_.zero();
            for (const auto& l: res)
              for (const auto& r: expand(c))
                ps_.add_weight(sum,
                               rs_.intersection(l.first, r.first),
                               ws_.mul(l.second, r.second));
            res = sum;
          }
        res_ = ps_.rmul(ps_.lmul(v.left_weight(), res), v.right_weight());
      }

      virtual void
      visit(const prod_t& v)
      {
        polynomial_t res = ps_.one();
        for (auto c: v)
          res = ps_.mul(res, expand(c));
        res_ = ps_.rmul(ps_.lmul(v.left_weight(), res), v.right_weight());
      }

      virtual void
      visit(const star_t& v)
      {
        // Recurse, but make it a star.
        v.sub()->accept(*this);
        res_ = polynomial_t{{rs_.star(ratexp(res_)), ws_.one()}};
        res_ = ps_.rmul(ps_.lmul(v.left_weight(), res_), v.right_weight());
      }

    private:
      ratexpset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      /// Polynomialset of ratexps.
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
      /// The result.
      polynomial_t res_;
    };

  } // rat::

  /// Expanding a typed ratexp shared_ptr.
  template <typename RatExpSet>
  typename RatExpSet::value_t
  expand(const RatExpSet& rs, const typename RatExpSet::value_t& e)
  {
    rat::expand_visitor<RatExpSet> expand{rs};
    return expand(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /*-------------------.
      | dyn::expand(exp).  |
      `-------------------*/
      template <typename RatExpSet>
      ratexp
      expand(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return make_ratexp(e.get_ratexpset(),
                           expand(e.get_ratexpset(), e.ratexp()));
      }

      REGISTER_DECLARE(expand, (const ratexp& e) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_EXPAND_HH
