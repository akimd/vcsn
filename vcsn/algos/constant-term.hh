#ifndef VCSN_ALGOS_CONSTANT_TERM_HH
# define VCSN_ALGOS_CONSTANT_TERM_HH

# include <set>

# include <vcsn/ctx/fwd.hh>
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
      using context_t = typename ratexpset_t::context_t;
      using ratexp_t = typename ratexpset_t::ratexp_t;
      using weight_t = typename ratexpset_t::weight_t;
      using weightset_t = typename ratexpset_t::weightset_t;

      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using prod_t = typename super_type::prod_t;
      using shuffle_t = typename super_type::shuffle_t;
      using intersection_t = typename super_type::intersection_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      constant_term_visitor(const ratexpset_t& rs)
        : ws_(*rs.weightset())
      {}

      weight_t
      operator()(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      void
      apply_weights(const inner_t& v)
      {
        res_ = ws_.mul(v.left_weight(), res_);
        res_ = ws_.mul(res_, v.right_weight());
      }

      /// Easy recursion.
      weight_t constant_term(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = ws_.zero();
      }

      virtual void
      visit(const one_t& v)
      {
        res_ = v.left_weight();
      }

      virtual void
      visit(const atom_t&)
      {
        res_ = ws_.zero();
      }

      virtual void
      visit(const sum_t& v)
      {
        weight_t res = ws_.zero();
        for (auto c: v)
          res = ws_.add(res, constant_term(c));
        res_ = std::move(res);
        apply_weights(v);
      }

      virtual void
      visit(const prod_t& v)
      {
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
        apply_weights(v);
      }

      virtual void
      visit(const intersection_t& v)
      {
        // FIXME: Code duplication with prod_t.
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
        apply_weights(v);
      }

      virtual void
      visit(const shuffle_t& v)
      {
        // FIXME: Code duplication with prod_t.
        weight_t res = ws_.one();
        for (auto c: v)
          res = ws_.mul(res, constant_term(c));
        res_ = std::move(res);
        apply_weights(v);
      }

      virtual void
      visit(const star_t& v)
      {
        res_ = ws_.star(constant_term(v.sub()));
        apply_weights(v);
      }

    private:
      //ratexpset_t ws_;
      weightset_t ws_;
      weight_t res_;
    };

  } // rat::

  template <typename RatExpSet>
  typename RatExpSet::weight_t
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
        return make_weight(*e.get_ratexpset().weightset(),
                           constant_term<RatExpSet>(e.get_ratexpset(),
                                                    e.ratexp()));
      }

      REGISTER_DECLARE(constant_term, (const ratexp& e) -> weight);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_CONSTANT_TERM_HH
