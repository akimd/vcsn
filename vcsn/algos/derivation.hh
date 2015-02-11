#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/split.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*--------------------------.
  | derivation(expression).   |
  `--------------------------*/

  namespace rat
  {
    template <typename ExpSet>
    class derivation_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using context_t = context_t_of<expressionset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_t = typename expressionset_t::const_visitor;
      using node_t = typename super_t::node_t;

      constexpr static const char* me() { return "derivation"; }

      derivation_visitor(const expressionset_t& rs)
        : rs_(rs)
      {}

      polynomial_t
      operator()(const expression_t& v, label_t var)
      {
        variable_ = var;
        v->accept(*this);
        return std::move(res_);
      }

      // FIXME: duplicate with expand.
      expression_t
      expression(const polynomial_t p)
      {
        expression_t res = rs_.zero();
        for (const auto& m: p)
          res = rs_.add(res, rs_.lmul(m.second, m.first));
         return res;
      }

      VCSN_RAT_UNSUPPORTED(ldiv)
      VCSN_RAT_UNSUPPORTED(transposition)

      VCSN_RAT_VISIT(zero,)
      {
        res_ = ps_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = ps_.zero();
      }

      VCSN_RAT_VISIT(atom, e)
      {
        if (e.value() == variable_)
          res_ = ps_.one();
        else
          res_ = ps_.zero();
      }

      VCSN_RAT_VISIT(sum, e)
      {
        polynomial_t res = ps_.zero();
        for (const auto& v: e)
          {
            v->accept(*this);
            ps_.add_here(res, res_);
          }
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(prod, e)
      {
        // We generate a sum.
        auto res = ps_.zero();
        // Accumulate the product of the constant terms of the
        // previous factors.
        weight_t constant = ws_.one();
        for (unsigned i = 0, n = e.size(); i < n; ++i)
          if (ws_.is_zero(constant))
            {
              // Finish the product with all the remaining rhs and
              // break.  This optimization (as opposed to performing
              // all the remaining iterations and repeatedly calling
              // ps.mul) improves the score for
              // derived-term([ab]*a[ab]{150}) from 0.32s to 0.23s on
              // erebus, clang++ 3.4.
              expression_t rhss = prod_(std::next(e.begin(), i), std::end(e));
              res_ = ps_.rmul(res_, rhss);
              break;
            }
          else
            {
              const auto& v = e[i];
              v->accept(*this);
              for (unsigned j = i + 1; j < n; ++j)
                res_ = ps_.rmul(res_, e[j]);
              ps_.add_here(res, ps_.lmul(constant, res_));
              constant = ws_.mul(constant, constant_term(rs_, v));
            }
        res_ = std::move(res);
      }

      /// Build a product for these expressions.  Pay attention to not
      /// building products with 0 or 1 expression.
      ///
      /// FIXME: Code duplication with expansion.
      expression_t
      prod_(typename prod_t::iterator begin,
            typename prod_t::iterator end) const
      {
        using expressions_t = typename prod_t::values_t;
        if (begin == end)
          return rs_.one();
        else if (std::next(begin, 1) == end)
          return *begin;
        else
          return std::make_shared<prod_t>(expressions_t{begin, end});
      }


      VCSN_RAT_VISIT(conjunction, e)
      {
        // The first polynomial.
        e.head()->accept(*this);
        auto res = res_;
        for (unsigned i = 1, n = e.size(); i < n; ++i)
          {
            const auto& v = e[i];
            v->accept(*this);
            res = ps_.conjunction(res, res_);
          }
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(shuffle, e)
      {
        polynomial_t res = ps_.zero();
        for (unsigned i = 0; i < e.size(); ++i)
          {
            e[i]->accept(*this);
            for (const auto& m: res_)
              {
                typename node_t::values_t expressions;
                for (unsigned j = 0; j < e.size(); ++j)
                  if (i == j)
                    expressions.emplace_back(m.first);
                  else
                    expressions.emplace_back(e[j]);
                // FIXME: we need better n-ary constructors.
                ps_.add_here(res,
                             std::make_shared<shuffle_t>(expressions),
                             m.second);
              }
          }
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(complement, e)
      {
        e.sub()->accept(*this);
        // Turn the polynomial into an expression, and complement it.
        res_ = polynomial_t{{rs_.complement(expression(res_)), ws_.one()}};
      }

      VCSN_RAT_VISIT(star, e)
      {
        e.sub()->accept(*this);
        // We need a copy of e, but without its weights.
        res_ = ps_.lmul(ws_.star(constant_term(rs_, e.sub())),
                        ps_.rmul(res_, e.shared_from_this()));
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.lmul(e.weight(), res_);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);
        polynomial_t res;
        for (const auto& m: res_)
          ps_.add_here(res, rs_.rmul(m.first, e.weight()), m.second);
        res_ = res;
      }

    private:
      expressionset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// The result.
      polynomial_t res_;
      /// The derivation variable.
      label_t variable_;
    };

  } // rat::

  /// Derive an expression wrt to a letter.
  template <typename ExpSet>
  inline
  rat::expression_polynomial_t<ExpSet>
  derivation(const ExpSet& rs,
             const typename ExpSet::value_t& e,
             label_t_of<ExpSet> a,
             bool breaking = false)
  {
    static_assert(ExpSet::context_t::labelset_t::is_free(),
                  "derivation: requires free labelset");
    rat::derivation_visitor<ExpSet> derivation{rs};
    auto res = derivation(e, a);
    if (breaking)
      res = split(rs, res);
    return res;
  }


  /// Derive a polynomial of expressions wrt to a letter.
  template <typename ExpSet>
  inline
  rat::expression_polynomial_t<ExpSet>
  derivation(const ExpSet& rs,
             const rat::expression_polynomial_t<ExpSet>& p,
             label_t_of<ExpSet> a,
             bool breaking = false)
  {
    auto ps = rat::make_expression_polynomialset(rs);
    using polynomial_t = rat::expression_polynomial_t<ExpSet>;
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res,
                   ps.lmul(m.second, derivation(rs, m.first, a, breaking)));
    return res;
  }


  /// Derive an expression wrt to a word.
  ///
  /// Do not enable this overload when labels and words are the same
  /// thing (e.g., when working on a wordset), since in this case we
  /// fail with an ambiguous overload.  Thanks to this enable_if, only
  /// the first overload (derivation wrt a label) is enabled, and the
  /// compilation fails but this time with the right diagnostic: the
  /// labelset must be free.
  template <typename ExpSet,
            typename = vcsn::enable_if_t<!std::is_same<word_t_of<ExpSet>,
                                                       label_t_of<ExpSet>>
                                         ::value>>
  inline
  rat::expression_polynomial_t<ExpSet>
  derivation(const ExpSet& rs,
             const typename ExpSet::value_t& e,
             const word_t_of<ExpSet>& l,
             bool breaking = false)
  {
    auto word = rs.labelset()->letters_of(l);
    auto i = std::begin(word);
    auto end = std::end(word);
    require(i != end, "derivation: word cannot be empty");
    auto res = derivation(rs, e, *i, breaking);
    for (++i; i != end; ++i)
      res = derivation(rs, res, *i, breaking);
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet, typename Label, typename Bool>
      inline
      polynomial
      derivation(const expression& exp, const label& lbl, bool breaking = false)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& l = lbl->as<Label>().label();
        const auto& rs = e.expressionset();
        auto ps = vcsn::rat::make_expression_polynomialset(rs);
        return make_polynomial(ps,
                               vcsn::derivation(rs, e.expression(),
                                                l, breaking));
      }

      REGISTER_DECLARE(derivation,
                       (const expression& e, const label& l,
                        bool breaking) -> polynomial);
    }
  }

} // vcsn::
