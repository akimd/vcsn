#ifndef VCSN_ALGOS_DERIVE_HH
# define VCSN_ALGOS_DERIVE_HH

# include <set>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/algos/constant-term.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace rat
  {

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

      ratexp_t
      operator()(const ratexp_t& v, letter_t var)
      {
        variable_ = var;
        v->accept(*this);
        return std::move(res_);
      }


      void
      apply_weights(const inner_t& e)
      {
        res_ = rs_.weight(e.left_weight(), res_);
        res_ = rs_.weight(res_, e.right_weight());
      }

      void
      apply_weights(const leaf_t& e)
      {
        res_ = rs_.weight(e.left_weight(), res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = rs_.zero();
      }

      virtual void
      visit(const one_t&)
      {
        res_ = rs_.zero();
      }

      virtual void
      visit(const atom_t& e)
      {
        if (e.value() == variable_)
          {
            res_ = rs_.one();
            apply_weights(e);
          }
        else
          res_ = rs_.zero();
      }

      virtual void
      visit(const sum_t& e)
      {
        ratexp_t res = rs_.zero();
        for (auto v: e)
          {
            v->accept(*this);
            res = rs_.add(res, res_);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const prod_t& e)
      {
        // We generate a sum.
        auto res = rs_.zero();
        // Accumulate the product of the constant terms of the
        // previous factors.
        weight_t constant = ws_.one();
        for (unsigned i = 0, n = e.size(); i < n; ++i)
          {
            const auto& v = e[i];
            v->accept(*this);
            for (unsigned j = i + 1; j < n; ++j)
              res_ = rs_.mul(res_, e[j]);
            res = rs_.add(res, rs_.weight(constant, res_));
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
        res_ = rs_.weight(ws_.star(constant_term(rs_, e.sub())),
                          rs_.mul(res_, e2));
        apply_weights(e);
      }

    private:
      ratexpset_t rs_;
      weightset_t ws_ = *rs_.weightset();
      ratexp_t res_;
      /// The derivation variable.
      letter_t variable_;
    };

  } // rat::

  template <typename RatExpSet>
  typename RatExpSet::ratexp_t
  derive(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
         char a)
  {
    rat::derive_visitor<RatExpSet> derive{rs};
    return derive(e, a);
  }

  template <typename RatExpSet>
  typename RatExpSet::ratexp_t
  derive(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
         const std::string& s)
  {
    rat::derive_visitor<RatExpSet> derive{rs};
    if (s.empty())
          throw std::runtime_error("cannot derive wrt an empty string");
    auto res = derive(e, s[0]);
    for (size_t i = 1, len = s.size(); i < len; ++i)
      res = derive(res, s[i]);
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
      ratexp
      derive(const ratexp& exp, const std::string& s)
      {
        const auto& e = exp->as<RatExpSet>();
        return make_ratexp(e.get_ratexpset(),
                           derive<RatExpSet>(e.get_ratexpset(),
                                             e.ratexp(), s));
      }

      REGISTER_DECLARE(derive,
                       (const ratexp& e, const std::string& s) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_DERIVE_HH
