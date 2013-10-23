#ifndef VCSN_ALGOS_EXPAND_HH
# define VCSN_ALGOS_EXPAND_HH

# include <set>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>

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
      using weight_t = typename context_t::weight_t;

      using super_type = typename RatExpSet::const_visitor;
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

      expand_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      ratexp_t
      operator()(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = rs_.zero();
      }

      virtual void
      visit(const one_t& v)
      {
        res_ = rs_.one(v.left_weight());
      }

      virtual void
      visit(const atom_t& v)
      {
        res_ = rs_.weight(v.left_weight(), rs_.atom(v.value()));
      }

      virtual void
      visit(const sum_t& v)
      {
        v.head()->accept(*this);
        ratexp_t res = res_;
        for (auto c: v.tail())
          {
            c->accept(*this);
            res = rs_.add(res, res_);
          }
        res_ = rs_.weight(rs_.weight(v.left_weight(),
                                     std::move(res)),
                          v.right_weight());
      }

      virtual void
      visit(const prod_t& v)
      {
        v.head()->accept(*this);
        ratexp_t res = res_;
        for (auto c: v.tail())
          {
            c->accept(*this);
            res = rs_.mul(res, res_);
          }
        res_ = rs_.weight(rs_.weight(v.left_weight(),
                                     std::move(res)),
                          v.right_weight());
      }

      virtual void
      visit(const star_t& v)
      {
        v.sub()->accept(*this);
        res_ = rs_.weight(rs_.weight(v.left_weight(),
                                     rs_.star(std::move(res_))),
                          v.right_weight());
      }

    private:
      ratexpset_t rs_;
      ratexp_t res_;
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
