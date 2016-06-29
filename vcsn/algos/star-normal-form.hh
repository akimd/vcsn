#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/algorithm.hh> // any_of
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{

  namespace rat
  {

    /*--------------------------------.
    | star_normal_form(expression).   |
    `--------------------------------*/

    /// \tparam ExpSet relative to the expression.
    ///
    /// Implementation based on the dot/box operators as defined in
    /// "On the Number of Broken Derived Terms of a Rational
    /// Expression", Pierre-Yves Angrand, Sylvain Lombardy, Jacques
    /// Sakarovitch.
    template <typename ExpSet>
    class star_normal_form_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using expression_t = typename expressionset_t::value_t;
      using context_t = context_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<context_t>;
      static_assert(std::is_same<weightset_t, b>::value,
                    "star_normal_form: requires Boolean weights");

      using weight_t = typename weightset_t::value_t;

      using super_t = typename ExpSet::const_visitor;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "star_normal_form"; }

      /// The type of the operator.
      enum operation_t { dot, box };

      star_normal_form_visitor(const expressionset_t& rs)
        : rs_(rs)
      {}

      expression_t
      operator()(const expression_t& v)
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
      VCSN_RAT_VISIT(add, v)
      {
        v.head()->accept(*this);
        expression_t res = res_;
        for (auto c: v.tail())
          {
            c->accept(*this);
            res = rs_.add(res, res_);
          }
        res_ = std::move(res);
      }

      VCSN_RAT_UNSUPPORTED(complement)
      VCSN_RAT_UNSUPPORTED(compose)
      VCSN_RAT_UNSUPPORTED(conjunction)
      VCSN_RAT_UNSUPPORTED(infiltrate)
      VCSN_RAT_UNSUPPORTED(ldivide)
      VCSN_RAT_UNSUPPORTED(shuffle)
      VCSN_RAT_UNSUPPORTED(transposition)
      VCSN_RAT_UNSUPPORTED(lweight)
      VCSN_RAT_UNSUPPORTED(rweight)
      using tuple_t = typename super_t::tuple_t;
      void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
      }

      VCSN_RAT_VISIT(mul, v)
      {
        if (operation_ == box)
          box_of(v);
        else
          dot_of(v);
      }

      /// Handling of a product by the box operator.
      void box_of(const mul_t& v)
      {
        using detail::any_of;
        if (any_of(v,
                   [this](const expression_t& n)
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
            expression_t res = res_;
            for (auto c: v.tail())
              {
                c->accept(*this);
                res = rs_.add(res, res_);
              }
            res_ = std::move(res);
          }
      }

      /// Handling of a product by the dot operator.
      void dot_of(const mul_t& v)
      {
        v.head()->accept(*this);
        expression_t res = res_;
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
            res_ = rs_.lweight(ws_.star(constant_term(rs_, v.sub())), res_);
            operation_ = dot;
          }
        else
          {
            v.sub()->accept(*this);
          }
      }

    private:
      expressionset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      /// The result.
      expression_t res_;
      /// The current operation.
      operation_t operation_ = dot;
    };

  } // rat::

  /// Star-normal form of an expression.
  template <typename ExpSet>
  typename ExpSet::value_t
  star_normal_form(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    auto snf = rat::star_normal_form_visitor<ExpSet>{rs};
    return snf(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      expression
      star_normal_form(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return {e.valueset(), ::vcsn::star_normal_form(e.valueset(),
                                                            e.value())};
      }
    }
  }

} // vcsn::
