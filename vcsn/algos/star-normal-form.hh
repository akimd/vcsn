#ifndef VCSN_ALGOS_STAR_NORMAL_FORM_HH
# define VCSN_ALGOS_STAR_NORMAL_FORM_HH

# include <vcsn/algos/constant-term.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace rat
  {

    /*---------------------------.
    | star_normal_form(ratexp).  |
    `---------------------------*/

    /// \tparam RatExpSet relative to the RatExp.
    ///
    /// Implementation based on the dot/box operators as defined in
    /// "On the Number of Broken Derived Terms of a Rational
    /// Expression", Pierre-Yves Angrand, Sylvain Lombardy, Jacques
    /// Sakarovitch.
    template <typename RatExpSet>
    class star_normal_form_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using ratexp_t = typename ratexpset_t::value_t;
      using context_t = typename ratexpset_t::context_t;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

      using super_type = typename RatExpSet::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using intersection_t = typename super_type::intersection_t;
      using shuffle_t = typename super_type::shuffle_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;
      using lweight_t = typename super_type::lweight_t;
      using rweight_t = typename super_type::rweight_t;

      /// The type of the operator.
      enum operation_t { dot, box };

      star_normal_form_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      ratexp_t
      operator()(const ratexp_t& v)
      {
        operation_ = dot;
        v->accept(*this);
        return std::move(res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = rs_.zero();
      }

      virtual void
      visit(const one_t&)
      {
        res_ = operation_ == box ? rs_.zero() : rs_.one();
      }

      virtual void
      visit(const atom_t& v)
      {
        res_ = rs_.atom(v.value());
      }

      // Plain traversal for sums.
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
        res_ = std::move(res);
      }

      virtual void
      visit(const intersection_t&)
      {
        throw std::domain_error("star_normal_form: intersection is not supported");
      }

      virtual void
      visit(const shuffle_t&)
      {
        throw std::domain_error("star_normal_form: shuffle is not supported");
      }

      /// Handling of a product by the box operator.
      void box_of(const prod_t& v)
      {
        if (std::any_of(std::begin(v), std::end(v),
                        [this](const ratexp_t& n)
                        {
                          return ws_.is_zero(constant_term(rs_, n));
                        }))
          {
            // Some factor has a null constant-term.
            operation_ = dot;
            dot_of(v);
            operation_ = box;
          }
        else
          {
            // All the factors have a non null constant-term.
            v.head()->accept(*this);
            ratexp_t res = res_;
            for (auto c: v.tail())
              {
                c->accept(*this);
                res = rs_.add(res, res_);
              }
            res_ = std::move(res);
          }
      }

      /// Handling of a product by the dot operator.
      void dot_of(const prod_t& v)
      {
        v.head()->accept(*this);
        ratexp_t res = res_;
        for (auto c: v.tail())
          {
            c->accept(*this);
            res = rs_.mul(res, res_);
          }
        res_ = std::move(res);
      }

      virtual void
      visit(const prod_t& v)
      {
        if (operation_ == box)
          box_of(v);
        else
          dot_of(v);
      }

      virtual void
      visit(const star_t& v)
      {
        if (operation_ == dot)
          {
            operation_ = box;
            v.sub()->accept(*this);
            res_ = rs_.star(res_);
            res_ = rs_.lmul(ws_.star(constant_term(rs_, v.sub())), res_);
            operation_ = dot;
          }
        else
          {
            v.sub()->accept(*this);
          }
      }

      virtual void
      visit(const lweight_t& v)
      {
        v.sub()->accept(*this);
        res_ = rs_.lmul(v.weight(), std::move(res_));
      }

      virtual void
      visit(const rweight_t& v)
      {
        v.sub()->accept(*this);
        res_ = rs_.rmul(std::move(res_), v.weight());
      }

    private:
      ratexpset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      /// The result.
      ratexp_t res_;
      /// The current operation.
      operation_t operation_ = dot;
    };

  } // rat::

  /// Star_Normal_Forming a typed ratexp shared_ptr.
  template <typename RatExpSet>
  typename RatExpSet::value_t
  star_normal_form(const RatExpSet& rs, const typename RatExpSet::value_t& e)
  {
    rat::star_normal_form_visitor<RatExpSet> star_normal_form{rs};
    return star_normal_form(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /*-----------------------------.
      | dyn::star_normal_form(exp).  |
      `-----------------------------*/
      template <typename RatExpSet>
      ratexp
      star_normal_form(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return make_ratexp(e.get_ratexpset(),
                           star_normal_form(e.get_ratexpset(), e.ratexp()));
      }

      REGISTER_DECLARE(star_normal_form, (const ratexp& e) -> ratexp);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_STAR_NORMAL_FORM_HH
