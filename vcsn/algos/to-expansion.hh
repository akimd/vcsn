#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/expansionset.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/expression-automaton.hh>
#include <vcsn/core/rat/visitor.hh>
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
        try
          {
            res_ = xs_.zero();
            v->accept(*this);
            return res_;
          }
        catch (const std::runtime_error& e)
          {
            raise(e, "  while computing expansion of: ", to_string(rs_, v));
          }
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
        auto insert = cache_.emplace(e, xs_.zero());
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
        auto res = xs_.zero();
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
      std::ostream&
      print_(const expansion_t& v, std::ostream& o = std::cerr) const
      {
        xs_.print(v, o);
        if (transposed_)
          o << " (transposed)";
        return o;
      }

      /// Find if an expression is named.
      expression_t me(expression_t res) const
      {
        while (true)
          {
            auto i = names_.find(res);
            if (i == end(names_))
              break;
            else
              res = i->second;
          }
         return res;
      }


      /// When we find that an expression is named, record `named ->
      /// naming`, so that when we need `named`, we actually use
      /// `naming`.  That's what the function `me` does.
      VCSN_RAT_VISIT(name, e)
      {
        // Record that e.sub has a name.
        auto i = names_.emplace(e.sub(), e.shared_from_this()).first;
        super_t::visit(e);
        names_.erase(i);
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = xs_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = xs_.one();
      }

      VCSN_RAT_VISIT(atom, e)
      {
        res_ = xs_.atom(transposed_
                        ? ls_.transpose(e.value())
                        : e.value());
      }

      VCSN_RAT_VISIT(add, e)
      {
        res_ = xs_.zero();
        for (const auto& v: e)
          xs_.add_here(res_, to_expansion(v));
      }

      VCSN_RAT_VISIT(mul, e)
      {
        if (getenv("VCSN_NAIVE_MUL"))
          {
            assert(!transposed_);
            res_ = to_expansion(e[0]);
            xs_.denormalize(res_);
            xs_.rmul_label_here(res_,
                                prod_(std::next(e.begin()), std::end(e)));
          }
        else
          {
        res_ = xs_.one();
        for (size_t i = 0, size = e.size(); i < size; ++i)
          {
            auto r = e[transposed_ ? size-1 - i : i];
            auto rhs = to_expansion(r);
            if (transposed_)
              r = rs_.transposition(r);

            // Instead of performing successive binary
            // multiplications, we immediately multiply the current
            // expansion by all the remaining operands on its right
            // hand side.  This also allows to break the iterations as
            // soon as an expansion has a null constant term.
            //
            // In essence, it allows to treat the product as if it
            // were right-associative: in `E.(FG)`, if `c(E) != 0`, we
            // don't need to traverse `FG`, just append it to
            // `d_p(E)`.
            //
            // Also, using prod_ saves from a useless application of
            // the identities with repeated multiplications, which
            // have already been applied.  Large performance impact.
            //
            // The gain is very effective.
            //
            // Do it only if there is really something left though, so
            // that we don't need any particular identity.
            if (i + 1 < size)
              {
                expression_t rhss
                  = transposed_
                  ? rs_.transposition(prod_(e.begin(),
                                            std::next(e.begin(), size-(i+1))))
                  : prod_(std::next(e.begin(), i + 1), std::end(e));
                // rmul_label_here requires a null constant term,
                // please it.
                auto w = std::move(rhs.constant);
                rhs.constant = ws_.zero();
                xs_.rmul_label_here(rhs, rhss);
                rhs.constant = std::move(w);
              }

            for (const auto& p: rhs.polynomials)
              ps_.add_here(res_.polynomials[p.first],
                           ps_.lweight(res_.constant, p.second));
            res_.constant = ws_.mul(res_.constant, rhs.constant);
            // Nothing else will be added.
            if (ws_.is_zero(res_.constant))
              break;
          }
          }
      }

      /// Build a product for these expressions.  Pay attention to not
      /// building products with 0 or 1 expression.
      ///
      /// FIXME: Code duplication with derivation.
      expression_t
      prod_(typename mul_t::iterator begin,
            typename mul_t::iterator end) const
      {
        using expressions_t = typename mul_t::values_t;
        assert(begin != end);
        if (std::next(begin, 1) == end)
          return *begin;
        else
          return std::make_shared<mul_t>(expressions_t{begin, end});
      }

      VCSN_RAT_VISIT(ldivide, e)
      {
        assert(e.size() == 2);
        bool transposed = transposed_;
        transposed_ = false;
        auto lhs = to_expansion(e[0]);
        auto rhs = to_expansion(e[1]);
        res_ = xs_.ldivide(lhs, rhs);
        if (transposed)
          res_ = xs_.transpose(res_);
        transposed_ = transposed;
      }

      /// d(E&F) = d(E) & d(F).
      VCSN_RAT_VISIT(conjunction, e)
      {
        res_ = to_expansion(e.head());
        for (const auto& r: e.tail())
          res_ = xs_.conjunction(res_, to_expansion(r));
      }

      /// d(E:F) = d(E):F + E:d(F)
      /// dᵗ(E:F) = dᵗ(E):Fᵗ + Eᵗ:dᵗ(F)
      VCSN_RAT_VISIT(shuffle, e)
      {
        // The shuffle-product of the previously traversed siblings,
        // to compute the "E:d(F)" part, E being all the previous lhs.
        auto prev = e.head();
        res_ = to_expansion(prev);
        for (const auto& r: e.tail())
          {
            res_ = xs_.shuffle(res_,
                               transposed_ ? rs_.transposition(prev) : prev,
                               to_expansion(r),
                               transposed_ ? rs_.transposition(r) : r);
            prev = rs_.shuffle(prev, r);
          }
      }

      /// d(E&:F) = d(E)&:F + d(E)&:d(F) + E&:d(F)
      /// dᵗ(E&:F) = dᵗ(E)&:Fᵗ + dᵗ(E)&:dᵗ(F) + Eᵗ&:dᵗ(F)
      VCSN_RAT_VISIT(infiltrate, e)
      {
        // The infiltration-product of the previously traversed
        // siblings, to compute the "E&:d(F)" part, E being all the
        // previous lhs.
        auto prev = e.head();
        res_ = to_expansion(prev);
        for (const auto& r: e.tail())
          {
            res_ = xs_.infiltrate
              (res_,
               transposed_ ? rs_.transposition(prev) : prev,
               to_expansion(r),
               transposed_ ? rs_.transposition(r) : r);
            prev = rs_.infiltrate(prev, r);
          }
      }

      VCSN_RAT_VISIT(complement, e)
      {
        res_ = xs_.complement(to_expansion(e.sub()));
      }

      /// d(Eᵗ) = dᵗ(E)
      VCSN_RAT_VISIT(transposition, e)
      {
        transposed_ = !transposed_;
        e.sub()->accept(*this);
        transposed_ = !transposed_;
      }

      /// If E is normal, d(E*) = <c(E)*> + <c(E)*> dp(E) E*.
      /// otherwise       d(E*) = 1 + d(E) E*.
      VCSN_RAT_VISIT(star, e)
      {
        // F = E*.
        auto f = me(e.shared_from_this());
        // d(E).
        auto x = to_expansion(e.sub());
        if (xs_.is_normal(x) && !getenv("VCSN_NAIVE_STAR"))
          res_.constant = ws_.star(x.constant);
        else
          {
            // Do not leave any constant-term.
            xs_.denormalize(x);
            res_.constant = ws_.one();
          }
        if (transposed_)
          {
            // FIXME: check the case of named expression.
            res_.constant = ws_.transpose(res_.constant);
            f = rs_.transposition(f);
          }

        for (const auto& p: x.polynomials)
          res_.polynomials[label_of(p)]
            = ps_.lweight(res_.constant,
                          ps_.rmul_label(weight_of(p), f));
        if (getenv("VCSN_DENORMALIZE_STAR"))
          xs_.denormalize(res_);
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        auto l = e.weight();
        auto r = to_expansion(e.sub());
        res_
          = transposed_
          ? xs_.rweight(r, ws_.transpose(l))
          : xs_.lweight_here(l, r);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        auto l = to_expansion(e.sub());
        auto r = e.weight();
        res_
          = transposed_
          ? xs_.lweight_here(ws_.transpose(r), l)
          : xs_.rweight(l, r);
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
            .xs_
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
          res_ = xs_.compose(res_, to_expansion(r));
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
      expansionset_t xs_ = {rs_};

      /// Whether to work transposed.
      bool transposed_ = false;
      /// The result.
      expansion_t res_ = xs_.zero();
      /// A table from the expression to the naming expression.
      std::map<expression_t, expression_t> names_;
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
