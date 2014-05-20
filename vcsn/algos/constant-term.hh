#ifndef VCSN_ALGOS_CONSTANT_TERM_HH
# define VCSN_ALGOS_CONSTANT_TERM_HH

# include <set>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/weight.hh>

namespace vcsn
{

  namespace rat
  {

    /*------------------------.
    | constant_term(ratexp).  |
    `------------------------*/

    /// \tparam RatExpSet  the ratexp set type.
    template <typename RatExpSet>
    class constant_term_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = context_t_of<ratexpset_t>;
      using ratexp_t = typename ratexpset_t::ratexp_t;
      using weight_t = weight_t_of<ratexpset_t>;
      using weightset_t = typename ratexpset_t::weightset_t;

      using super_type = typename ratexpset_t::const_visitor;

      constexpr static const char* me() { return "constant_term"; }

      constant_term_visitor(const ratexpset_t& rs)
        : ws_(*rs.weightset())
      {}

      weight_t
      operator()(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      /// Easy recursion.
      weight_t constant_term(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = ws_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = ws_.one();
      }

      VCSN_RAT_VISIT(atom,)
      {
        res_ = ws_.zero();
      }

      VCSN_RAT_VISIT(sum, v)
      {
        weight_t res = ws_.zero();
        for (auto c: v)
          res = ws_.add(res, constant_term(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(prod, v)
      {
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
      }

      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(transposition)

      VCSN_RAT_VISIT(conjunction, v)
      {
        // FIXME: Code duplication with prod_t.
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(shuffle, v)
      {
        // FIXME: Code duplication with prod_t.
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(star, v)
      {
        res_ = ws_.star(constant_term(v.sub()));
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        v.sub()->accept(*this);
        res_ = ws_.mul(v.weight(), constant_term(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        v.sub()->accept(*this);
        res_ = ws_.mul(constant_term(v.sub()), v.weight());
      }

      VCSN_RAT_VISIT(complement, v)
      {
        res_
          = ws_.is_zero(constant_term(v.sub()))
          ? ws_.one()
          : ws_.zero();
      }

    private:
      //ratexpset_t ws_;
      weightset_t ws_;
      weight_t res_;
    };

  } // rat::

  template <typename RatExpSet>
  weight_t_of<RatExpSet>
  constant_term(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e)
  {
    rat::constant_term_visitor<RatExpSet> constant_term{rs};
    return constant_term(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /*--------------------------.
      | dyn::constant_term(exp).  |
      `--------------------------*/
      template <typename RatExpSet>
      weight
      constant_term(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return make_weight(*e.ratexpset().weightset(),
                           constant_term<RatExpSet>(e.ratexpset(),
                                                    e.ratexp()));
      }

      REGISTER_DECLARE(constant_term, (const ratexp& e) -> weight);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_CONSTANT_TERM_HH
