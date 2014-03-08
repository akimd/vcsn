#ifndef VCSN_ALGOS_STAR_NORMAL_FORM_HH
# define VCSN_ALGOS_STAR_NORMAL_FORM_HH

# include <vcsn/algos/constant-term.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/raise.hh>

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

      constexpr static const char* me() { return "star_normal_form"; }

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

      VCSN_RAT_VISIT(zero,)
      {
        res_ = rs_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = operation_ == box ? rs_.zero() : rs_.one();
      }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = rs_.atom(v.value());
      }

      // Plain traversal for sums.
      VCSN_RAT_VISIT(sum, v)
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

      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(intersection)
      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)

      VCSN_RAT_VISIT(prod, v)
      {
        if (operation_ == box)
          box_of(v);
        else
          dot_of(v);
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

      VCSN_RAT_VISIT(star, v)
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

      VCSN_RAT_VISIT(lweight, v)
      {
        v.sub()->accept(*this);
        res_ = rs_.lmul(v.weight(), std::move(res_));
      }

      VCSN_RAT_VISIT(rweight, v)
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
      /// Bridge.
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
