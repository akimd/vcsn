#pragma once

#include <vcsn/ctx/fwd.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/dyn/value.hh>

#include <vcsn/algos/derivation.hh> // expression_polynomialset_t.

namespace vcsn
{

  namespace rat
  {

    /*----------------------.
    | expand(expression).   |
    `----------------------*/

    /// \tparam ExpSet  relative to the expression.
    template <typename ExpSet>
    class expand_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using expression_t = typename expressionset_t::value_t;
      using context_t = context_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_t = typename ExpSet::const_visitor;

      constexpr static const char* me() { return "expand"; }

      expand_visitor(const expressionset_t& rs)
        : rs_(rs)
      {}

      expression_t
      operator()(const expression_t& v)
      {
        v->accept(*this);
        return ps_.to_label(res_);
      }

    private:
      /// Syntactic sugar: recursive call to this visitor.
      polynomial_t expand(const expression_t& e)
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

      VCSN_RAT_VISIT(conjunction, v)
      {
        auto res = expand(v.head());
        for (auto c: v.tail())
          {
            polynomial_t sum = ps_.zero();
            for (const auto& l: res)
              for (const auto& r: expand(c))
                ps_.add_here(sum,
                             rs_.conjunction(label_of(l), label_of(r)),
                             ws_.mul(weight_of(l), weight_of(r)));
            res = sum;
          }
        res_ = std::move(res);
      }

      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(infiltration)
      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)

      using tuple_t = typename super_t::tuple_t;
      virtual void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
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
        res_ = polynomial_t{{rs_.star(ps_.to_label(res_)), ws_.one()}};
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
      expressionset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      /// Polynomialset of expressions.
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// The result.
      polynomial_t res_;
    };

  } // rat::

  /// Expand a typed expression.
  template <typename ExpSet>
  inline
  typename ExpSet::value_t
  expand(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    rat::expand_visitor<ExpSet> expand{rs};
    return expand(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      inline
      expression
      expand(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return {e.valueset(), ::vcsn::expand(e.valueset(), e.value())};
      }
    }
  }

} // vcsn::
