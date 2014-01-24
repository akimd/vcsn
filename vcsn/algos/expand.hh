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
          res = rs_.add(res, rs_.lmul(m.second, m.first));
         return res;
      }

      /// Syntactic sugar: recursive call to this visitor.
      polynomial_t expand(const ratexp_t& e)
      {
        e->accept(*this);
        return res_;
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = ps_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = polynomial_t{{rs_.one(), ws_.one()}};
      }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = polynomial_t{{rs_.atom(v.value()), ws_.one()}};
      }

      VCSN_RAT_VISIT(sum, v)
      {
        polynomial_t res = ps_.zero();
        for (auto c: v)
          res = ps_.add(res, expand(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(intersection, v)
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
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(shuffle,)
      {
        raise("expand: shuffle is not supported");
      }

      VCSN_RAT_VISIT(complement,)
      {
        raise("expand: complement is not supported");
      }

      VCSN_RAT_VISIT(prod, v)
      {
        polynomial_t res = ps_.one();
        for (auto c: v)
          res = ps_.mul(res, expand(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(star, v)
      {
        // Recurse, but make it a star.
        v.sub()->accept(*this);
        res_ = polynomial_t{{rs_.star(ratexp(res_)), ws_.one()}};
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        v.sub()->accept(*this);
        res_ = ps_.lmul(v.weight(), std::move(res_));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        v.sub()->accept(*this);
        res_ = ps_.rmul(std::move(res_), v.weight());
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
      /// Bridge.
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
