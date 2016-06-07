#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/expansionset.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/core/expression-automaton.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/map.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/polynomialset.hh>

//# define DEBUG 1

#if DEBUG
# define DEBUG_IFELSE(Then, Else) Then
#else
# define DEBUG_IFELSE(Then, Else) Else
#endif

#define DEBUG_IF(Then) DEBUG_IFELSE(Then,)

namespace vcsn
{

  namespace rat
  {

    /*------------------------.
    | to_expansion_visitor.   |
    `------------------------*/

    /// Functor to compute the expansion of an expression.
    ///
    /// \tparam ExpSet  the expressionset type.
    template <typename ExpSet>
    class to_expansion_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using self_t = to_expansion_visitor;

      using context_t = context_t_of<expressionset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = typename weightset_t::value_t;
      using expansionset_t = expansionset<expressionset_t>;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      constexpr static const char* me() { return "to_expansion"; }

      using polys_t = typename expansionset_t::polys_t;
      using expansion_t = typename expansionset_t::value_t;

      to_expansion_visitor(const expressionset_t& rs)
        : rs_(rs)
      {}

      /// From an expression, build its expansion.
      expansion_t operator()(const expression_t& v)
      {
        res_ = es_.zero();
        v->accept(*this);
        return res_;
      }

    private:
#if CACHE
      std::unordered_map<expression_t, expansion_t,
                         vcsn::hash<expressionset_t>,
                         vcsn::equal_to<expressionset_t>> cache_;
#endif
      /// Facilitate recursion.
      ///
      /// Saves and restore res_.
      expansion_t to_expansion(const expression_t& e)
      {
#if CACHE
        auto insert = cache_.emplace(e, es_.zero());
        auto& res = insert.first->second;
        if (insert.second)
          {
            std::swap(res, res_);
            DEBUG_IF(rs_.print(e, std::cerr) << "..." << incendl);
            e->accept(*this);
            std::swap(res, res_);
            DEBUG_IF(
                     rs_.print(e, std::cerr) << " => ";
                     print_(res, std::cerr) << decendl;
                     );
          }
        else
          {
            DEBUG_IF(
                     rs_.print(e, std::cerr) << " -> ";
                     print_(res, std::cerr) << iendl;
                     );
          }
        return res;
#else
        auto res = es_.zero();
        std::swap(res, res_);
        e->accept(*this);
        std::swap(res, res_);
        DEBUG_IF(
                 rs_.print(e, std::cerr) << " -> ";
                 print_(res, std::cerr) << iendl;
                 );
        return res;
#endif
      }

      /// Print an expansion.
      std::ostream& print_(const expansion_t& v, std::ostream& o) const
      {
        es_.print(v, o);
        if (transposed_)
          o << " (transposed)";
        return o;
      }

    private:
      VCSN_RAT_VISIT(zero,)
      {
        res_ = es_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = es_.one();
      }

      VCSN_RAT_VISIT(atom, e)
      {
        res_ = es_.atom(transposed_
                        ? ls_.transpose(e.value())
                        : e.value());
      }

      VCSN_RAT_VISIT(add, e)
      {
        res_ = es_.zero();
        for (const auto& v: e)
          es_.add_here(res_, to_expansion(v));
      }

      VCSN_RAT_VISIT(prod, e)
      {
        res_ = es_.one();
        for (size_t i = 0, size = e.size(); i < size; ++i)
          {
            auto r = e[transposed_ ? size-1 - i : i];
            expansion_t rhs = to_expansion(r);
            if (transposed_)
              r = rs_.transposition(r);

            // Instead of performing successive binary
            // multiplications, we immediately multiply the current
            // expansion by all the remaining operands on its right
            // hand side.  This will also allow us to break the
            // iterations as soon as an expansion has a null constant
            // term.
            //
            // In essence, it allows us to treat the product as if it
            // were right-associative: in `E.(FG)`, if `c(E) != 0`, we
            // don't need to traverse `FG`, just append it to
            // `d_p(E)`.
            //
            // Also, using prod_ saves from a useless application of
            // the identities with repeated multiplications, which
            // have already been applied.  Large performance impact.
            //
            // The gain is very effective.
            expression_t rhss
              = transposed_
              ? rs_.transposition(prod_(e.begin(),
                                        std::next(e.begin(), size-(i+1))))
              : prod_(std::next(e.begin(), i + 1), std::end(e));
            es_.rmul_here(rhs, rhss);

            for (const auto& p: rhs.polynomials)
              ps_.add_here(res_.polynomials[p.first],
                           ps_.lmul(res_.constant, p.second));
            res_.constant = ws_.mul(res_.constant, rhs.constant);
            // Nothing else will be added.
            if (ws_.is_zero(res_.constant))
              break;
          }
      }

      /// Build a product for these expressions.  Pay attention to not
      /// building products with 0 or 1 expression.
      ///
      /// FIXME: Code duplication with derivation.
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

      VCSN_RAT_VISIT(ldiv, e)
      {
        assert(e.size() == 2);
        DEBUG_IF(
                 std::cerr << "Start: ";
                 rs_.print(e.shared_from_this(), std::cerr) << " =>\n";
                 );

        bool transposed = transposed_;
        transposed_ = false;
        expansion_t lhs = to_expansion(e[0]);
        expansion_t rhs = to_expansion(e[1]);
        transposed_ = transposed;
        DEBUG_IF(
                 std::cerr << "Lhs: "; print_(lhs, std::cerr) << '\n';
                 std::cerr << "Rhs: "; print_(rhs, std::cerr) << '\n';
                 );
        res_ = es_.zero();
        auto one = detail::label_one(ls_);
        for (const auto& p: zip_maps(lhs.polynomials, rhs.polynomials))
          for (const auto& lm: std::get<0>(p.second))
            for (const auto& rm: std::get<1>(p.second))
              // Now, recursively develop the quotient of monomials,
              // directly in res_.
              if (transposed_)
                ps_.add_here(res_.polynomials[one],
                             rs_.transposition(rs_.ldiv(label_of(lm),
                                                        label_of(rm))),
                             ws_.transpose(ws_.ldiv(weight_of(lm),
                                                    weight_of(rm))));
              else
                ps_.add_here(res_.polynomials[one],
                             rs_.ldiv(label_of(lm), label_of(rm)),
                             ws_.ldiv(weight_of(lm), weight_of(rm)));
          if (has(lhs.polynomials, one))
            for (const auto& rhsp: rhs.polynomials)
              if (rhsp.first != one)
                for (const auto& lm: lhs.polynomials[one])
                  for (const auto& rm: rhsp.second)
                    {
                      if (transposed_)
                        ps_.add_here(res_.polynomials[one],
                          rs_.transposition(
                            rs_.ldiv(label_of(lm),
                                     rs_.mul(rs_.atom(rhsp.first),
                                             label_of(rm)))),
                          ws_.transpose(ws_.ldiv(weight_of(lm),
                              weight_of(rm))));
                      else
                        ps_.add_here(res_.polynomials[one],
                          rs_.ldiv(label_of(lm),
                            rs_.mul(rs_.atom(rhsp.first), label_of(rm))),
                          ws_.ldiv(weight_of(lm), weight_of(rm)));
                    }
          if (has(rhs.polynomials, one))
            for (const auto& lhsp: lhs.polynomials)
              if (lhsp.first != one)
                for (const auto& lm: lhsp.second)
                  for (const auto& rm: rhs.polynomials[one])
                    {
                      if (transposed_)
                        ps_.add_here(res_.polynomials[one],
                          rs_.transposition(
                            rs_.ldiv(rs_.mul(rs_.atom(lhsp.first),
                                             label_of(lm)),
                                     label_of(rm))),
                          ws_.transpose(ws_.ldiv(weight_of(lm),
                                        weight_of(rm))));
                      else
                        ps_.add_here(res_.polynomials[one],
                          rs_.ldiv(rs_.mul(rs_.atom(lhsp.first), label_of(lm)),
                            label_of(rm)),
                          ws_.ldiv(weight_of(lm), weight_of(rm)));
                    }
        if (!ws_.is_zero(lhs.constant))
          {
            if (transposed_)
              {
                auto rhs_transposed = to_expansion(e[1]);
                es_.add_here(res_,
                             es_.ldiv_here(lhs.constant, rhs_transposed));
              }
            else
              es_.add_here(res_, es_.ldiv_here(lhs.constant, rhs));
          }
        es_.normalize(res_);
      }

      // d(E&F) = d(E) & d(F).
      VCSN_RAT_VISIT(conjunction, e)
      {
        res_ = to_expansion(e.head());
        for (const auto& r: e.tail())
          res_ = es_.conjunction(res_, to_expansion(r));
      }

      // d(E:F) = d(E):F + E:d(F)
      VCSN_RAT_VISIT(shuffle, e)
      {
        // The shuffle-product of the previously traversed siblings,
        // to compute the "E:d(F)" part, E being all the previous lhs.
        auto prev = e.head();
        res_ = to_expansion(prev);
        for (const auto& r: e.tail())
          {
            res_ = es_.shuffle(res_, prev,
                               to_expansion(r), r);
            prev = rs_.shuffle(prev, r);
          }
      }

      // d(E&:F) = d(E)&:F + d(E)&:d(F) + E&:d(F)
      VCSN_RAT_VISIT(infiltration, e)
      {
        // The infiltration-product of the previously traversed
        // siblings, to compute the "E&:d(F)" part, E being all the
        // previous lhs.
        auto prev = e.head();
        res_ = to_expansion(prev);
        for (const auto& r: e.tail())
          {
            res_ = es_.infiltration(res_, prev,
                                    to_expansion(r), r);
            prev = rs_.infiltration(prev, r);
          }
      }

      VCSN_RAT_VISIT(complement, e)
      {
        res_ = es_.complement(to_expansion(e.sub()));
      }


      VCSN_RAT_VISIT(transposition, e)
      {
        transposed_ = !transposed_;
        e.sub()->accept(*this);
        transposed_ = !transposed_;
      }

      VCSN_RAT_VISIT(star, e)
      {
        expansion_t res = to_expansion(e.sub());
        res_.constant = ws_.star(res.constant);
        auto f = e.shared_from_this();
        if (transposed_)
          {
            res_.constant = ws_.transpose(res_.constant);
            f = rs_.transposition(f);
          }

        for (const auto& p: res.polynomials)
          res_.polynomials[label_of(p)]
            = ps_.lmul(res_.constant,
                       ps_.rmul_label(weight_of(p), f));
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        auto l = e.weight();
        auto r = to_expansion(e.sub());
        res_
          = transposed_
          ? es_.rmul(r, ws_.transpose(l))
          : es_.lmul_here(l, r);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        auto l = to_expansion(e.sub());
        auto r = e.weight();
        res_
          = transposed_
          ? es_.lmul_here(ws_.transpose(r), l)
          : es_.rmul(l, r);
      }

      using tuple_t = typename super_t::tuple_t;

      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        template <size_t... I>
        expansion_t work_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return
            visitor_
            .es_
            .tuple(vcsn::to_expansion(detail::project<I>(visitor_.rs_),
                                      std::get<I>(v.sub()))...);
        }

        expansion_t operator()(const tuple_t& v)
        {
          return work_(v, labelset_t_of<context_t>::indices);
        }
        const self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        expansion_t operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        const self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        res_ = visit_tuple<>{*this}(v);
      }

      // d(E@F) = d(E)@d(F).
      VCSN_RAT_VISIT(compose, e)
      {
        compose(e, 0);
      }

      template <typename Exp = expansion_t>
      auto compose(const compose_t& e, int)
        -> decltype(std::declval<expansionset_t>()
                    .compose(std::declval<Exp>(), std::declval<Exp>()),
                    void())
      {
        res_ = to_expansion(e.head());
        for (const auto& r: e.tail())
          res_ = es_.compose(res_, to_expansion(r));
      }

      auto compose(const compose_t&, long)
        -> void
      {
        require(false, "compose: context is not composable");
      }


      /// Manipulate the expressions.
      expressionset_t rs_;
      /// Manipulate the labels.
      labelset_t ls_ = *rs_.labelset();
      /// Manipulate the weights.
      weightset_t ws_ = *rs_.weightset();
      /// Manipulate the polynomials of expressions.
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// Manipulate the expansions.
      expansionset_t es_ = {rs_};

      /// Whether to work transposed.
      bool transposed_ = false;
      /// The result.
      expansion_t res_;
    };
  } // rat::

  /// First order expansion.
  template <typename ExpSet>
  typename rat::expansionset<ExpSet>::value_t
  to_expansion(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    auto to_expansion = rat::to_expansion_visitor<ExpSet>{rs};
    return to_expansion(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      expansion
      to_expansion(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& rs = e.valueset();
        auto es = vcsn::rat::expansionset<ExpSet>(rs);
        return {es, to_expansion<ExpSet>(rs, e.value())};
      }
    }
  }
} // vcsn::
