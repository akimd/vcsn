#pragma once

#include <vcsn/ctx/fwd.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/weight.hh>

namespace vcsn
{

  namespace rat
  {

    /*-----------------------------.
    | constant_term(expression).   |
    `-----------------------------*/

    /// \tparam ExpSet  the expression set type.
    template <typename ExpSet>
    class constant_term_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using context_t = context_t_of<expressionset_t>;
      using expression_t = typename expressionset_t::value_t;
      using weight_t = weight_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<expressionset_t>;

      using super_t = typename expressionset_t::const_visitor;

      constexpr static const char* me() { return "constant_term"; }

      constant_term_visitor(const expressionset_t& rs)
        : ws_(*rs.weightset())
      {}

      weight_t
      operator()(const expression_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      /// Easy recursion.
      weight_t constant_term(const expression_t& v)
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
      weightset_t ws_;
      weight_t res_;
    };

  } // rat::

  template <typename ExpSet>
  weight_t_of<ExpSet>
  constant_term(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    rat::constant_term_visitor<ExpSet> constant_term{rs};
    return constant_term(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      weight
      constant_term(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return make_weight(*e.expressionset().weightset(),
                           constant_term<ExpSet>(e.expressionset(),
                                                    e.expression()));
      }
    }
  }

} // vcsn::
