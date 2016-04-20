#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/split.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*--------------------------.
  | derivation(expression).   |
  `--------------------------*/

  template <typename ExpSet>
  rat::expression_polynomial_t<ExpSet>
  derivation(const ExpSet& rs,
             const typename ExpSet::value_t& e,
             label_t_of<ExpSet> a,
             bool breaking = false);

  namespace rat
  {
    /// Functor to compute the derivation of an expression.
    ///
    /// \tparam ExpSet  the expressionset type.
    template <typename ExpSet>
    class derivation_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using self_t = derivation_visitor;

      using context_t = context_t_of<expressionset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

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

    private:
      VCSN_RAT_UNSUPPORTED(infiltration)
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
          {
            const auto& v = e[i];
            v->accept(*this);
            // See to-expansion.hh, case of prod, for an explanation
            // of the following line.
            res_
              = ps_.rmul_label(res_,
                               prod_(std::next(e.begin(), i+1), std::end(e)));
            ps_.add_here(res, ps_.lmul(constant, res_));
            constant = ws_.mul(constant, constant_term(rs_, v));
            if (ws_.is_zero(constant))
              break;
          }
        res_ = std::move(res);
      }

      /// Build a product for these expressions.  Pay attention to not
      /// building products with 0 or 1 expression.
      ///
      /// FIXME: Code duplication with to_expansion.
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
                    expressions.emplace_back(label_of(m));
                  else
                    expressions.emplace_back(e[j]);
                // FIXME: we need better n-ary constructors.
                ps_.add_here(res,
                             std::make_shared<shuffle_t>(expressions),
                             weight_of(m));
              }
          }
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(complement, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.complement(res_);
      }

      VCSN_RAT_VISIT(star, e)
      {
        e.sub()->accept(*this);
        // We need a copy of e, but without its weights.
        res_ = ps_.lmul(ws_.star(constant_term(rs_, e.sub())),
                        ps_.rmul_label(res_, e.shared_from_this()));
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.lmul(e.weight(), res_);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        e.sub()->accept(*this);
        res_ = ps_.rmul(res_, e.weight());
      }

      /*---------.
      | tuple.   |
      `---------*/

      using tuple_t = typename super_t::tuple_t;
      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Tuple of derivations of all the tapes.
        template <size_t... I>
        polynomial_t work_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return
            visitor_
            .ps_
            .tuple(vcsn::derivation(detail::project<I>(visitor_.rs_),
                                    std::get<I>(v.sub()),
                                    std::get<I>(visitor_.variable_))...);
        }

        /// Entry point.
        polynomial_t operator()(const tuple_t& v)
        {
          return work_(v, labelset_t_of<context_t>::indices);
        }

        const self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        polynomial_t operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        const self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        res_ = visit_tuple<>{*this}(v);
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
  rat::expression_polynomial_t<ExpSet>
  derivation(const ExpSet& rs,
             const typename ExpSet::value_t& e,
             label_t_of<ExpSet> a,
             bool breaking)
  {
    static_assert(ExpSet::context_t::labelset_t::is_free(),
                  "derivation: requires free labelset");
    auto derivation = rat::derivation_visitor<ExpSet>{rs};
    auto res = derivation(e, a);
    if (breaking)
      res = split(rs, res);
    return res;
  }


  /// Derive a polynomial of expressions wrt to a letter.
  template <typename ExpSet>
  rat::expression_polynomial_t<ExpSet>
  derivation(const ExpSet& rs,
             const rat::expression_polynomial_t<ExpSet>& p,
             label_t_of<ExpSet> a,
             bool breaking = false)
  {
    auto ps = rat::make_expression_polynomialset(rs);
    using polynomial_t = rat::expression_polynomial_t<ExpSet>;
    auto res = polynomial_t{};
    for (const auto& m: p)
      res = ps.add(res,
                   ps.lmul(weight_of(m),
                           derivation(rs, label_of(m), a, breaking)));
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
            typename = std::enable_if_t<!std::is_same<word_t_of<ExpSet>,
                                                       label_t_of<ExpSet>>
                                         ::value>>
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
      polynomial
      derivation(const expression& exp, const label& lbl, bool breaking)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& l = lbl->as<Label>().value();
        const auto& rs = e.valueset();
        auto ps = vcsn::rat::make_expression_polynomialset(rs);
        return make_polynomial(ps,
                               vcsn::derivation(rs, e.value(), l, breaking));
      }
    }
  }
} // vcsn::
