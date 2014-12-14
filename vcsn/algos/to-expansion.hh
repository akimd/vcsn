#ifndef VCSN_ALGOS_TO_EXPANSION_HH
# define VCSN_ALGOS_TO_EXPANSION_HH

# include <stack>

# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/core/rat/expansionset.hh>
# include <vcsn/core/rat/expressionset.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/core/expression-automaton.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/expansion.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/expression.hh>
# include <vcsn/misc/indent.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/weightset/polynomialset.hh>

//# define DEBUG 1

#if DEBUG
# define DEBUG_IFELSE(Then, Else) Then
#else
# define DEBUG_IFELSE(Then, Else) Else
#endif

# define DEBUG_IF(Then) DEBUG_IFELSE(Then,)

namespace vcsn
{

  namespace rat
  {

    /*------------------------.
    | to_expansion_visitor.   |
    `------------------------*/

    template <typename ExpSet>
    class to_expansion_visitor
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
      using expansionset_t = expansionset<expressionset_t>;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      using super_t = typename expressionset_t::const_visitor;

      constexpr static const char* me() { return "to_expansion"; }

      using polys_t = typename expansionset_t::polys_t;
      using expansion_t = typename expansionset_t::value_t;

      to_expansion_visitor(const expressionset_t& rs)
        : rs_(rs)
      {}

      expansion_t operator()(const expression_t& v)
      {
        res_ = es_.zero();
        v->accept(*this);
        return res_;
      }

#if CACHE
      std::unordered_map<expression_t, expansion_t,
                         vcsn::hash<expressionset_t>,
                         vcsn::equal_to<expressionset_t>> cache_;
#endif

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

      polynomial_t to_expansion_as_polynomial(const expression_t& e)
      {
        operator()(e);
        return es_.as_polynomial(res_);
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

      VCSN_RAT_VISIT(sum, e)
      {
        res_ = es_.zero();
        for (const auto& v: e)
          es_.add_here(res_, to_expansion(v));
      }

      VCSN_RAT_VISIT(prod, e)
      {
        res_ = es_.one();
        for (size_t i = 0, size = e.size(); i < size; ++i)
          if (ws_.is_zero(res_.constant))
            {
              // Finish the product with all the remaining rhs and
              // break.  This optimization (as opposed to performing
              // all the remaining iterations and repeatedly calling
              // ps.mul) improves the score for
              // derived-term([ab]*a[ab]{150}) from 0.32s to 0.23s on
              // erebus, clang++ 3.4.
              expression_t rhss
                = transposed_
                ? rs_.transposition(prod_(e.begin(),
                                          std::next(e.begin(), size-i)))
                : prod_(std::next(e.begin(), i), std::end(e));
              es_.rmul_here(res_, rhss);
              break;
            }
          else
            {
              auto r = e[transposed_ ? size-1 - i : i];
              expansion_t rhs = to_expansion(r);
              if (transposed_)
                r = rs_.transposition(r);

              es_.rmul_here(res_, r);

              for (const auto& p: rhs.polynomials)
                ps_.add_here(res_.polynomials[p.first],
                             ps_.lmul(res_.constant, p.second));
              res_.constant = ws_.mul(res_.constant, rhs.constant);
            }
      }

      /// Build a product for these expressions.  Pay attention to not
      /// building products with 0 or 1 expression.
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

      label_t one_(std::true_type)
      {
        return rs_.labelset()->one();
      }

      label_t one_(std::false_type)
      {
        raise(me(), ": quotient requires a labelset with a neutral");
      }

      /// If r is e*, return e.
      /// If r is e*{T}, return e{T}.
      /// Otherwise return nullptr.
      ///
      /// FIXME: What about complement?
      expression_t star_child(const expression_t r)
      {
        if (auto c = std::dynamic_pointer_cast<const transposition_t>(r))
          {
            if (auto s = std::dynamic_pointer_cast<const star_t>(c->sub()))
              return rs_.transposition(s->sub());
          }
        else if (auto s = std::dynamic_pointer_cast<const star_t>(r))
          return s->sub();
        return nullptr;
      }

      VCSN_RAT_VISIT(ldiv, e)
      {
        assert(e.size() == 2);
        DEBUG_IF(
                 std::cerr << "Start: ";
                 rs_.print(e.shared_from_this, std::cerr()) << " =>\n";
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
        // lp: (label, left_polynomial)
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
        auto one = one_(std::integral_constant<bool, context_t::has_one()>());
        for (const auto& p: zip_maps(lhs.polynomials, rhs.polynomials))
          for (const auto& lm: std::get<0>(p.second))
            for (const auto& rm: std::get<1>(p.second))
              // Now, recursively develop the quotient of monomials,
              // directly in res_.
              if (transposed_)
                ps_.add_here(res_.polynomials[one],
                             rs_.transposition(rs_.ldiv(lm.first, rm.first)),
                             ws_.transpose(ws_.ldiv(lm.second, rm.second)));
              else
                ps_.add_here(res_.polynomials[one],
                             rs_.ldiv(lm.first, rm.first),
                             ws_.ldiv(lm.second, rm.second));
        es_.normalize(res_);
      }

      VCSN_RAT_VISIT(conjunction, e)
      {
        res_ = to_expansion(e.head());
        for (const auto& r: e.tail())
          res_ = es_.conjunction(res_, to_expansion(r));
      }

      // FO(E:F) = FO(E):F + E:FO(F)
      VCSN_RAT_VISIT(shuffle, e)
      {
        expansion_t res = es_.one();
        // The shuffle-product of the previously traversed siblings.
        // Initially the neutral element: \e.
        expression_t prev = rs_.one();
        for (const auto& rhs: e)
          {
            // Save current result in lhs, and compute the result in res.
            expansion_t lhs; lhs.constant = ws_.zero();
            std::swap(res, lhs);

            expansion_t r = to_expansion(rhs);
            res.constant = ws_.mul(lhs.constant, r.constant);

            // (i) fo(lhs) -> fo(lhs):r, that is, shuffle-multiply the
            // current result by the current child (rhs).
            for (const auto& p: lhs.polynomials)
              for (const auto& m: p.second)
                res.polynomials[p.first].emplace(rs_.shuffle(m.first, rhs),
                                                 m.second);
            // (ii) prev:fo(rhs)
            for (const auto& p: r.polynomials)
              for (const auto& m: p.second)
                ps_.add_here(res.polynomials[p.first],
                               rs_.shuffle(prev, m.first), m.second);

            prev = rs_.shuffle(prev, rhs);
          }
        res_ = res;
      }

      VCSN_RAT_VISIT(complement, e)
      {
        // Complement requires a free labelset.
        visit_complement<context_t::labelset_t::is_free()>(e);
      }

      /// Cannot complement on a non-free labelset.
      template <bool IsFree>
      vcsn::enable_if_t<!IsFree, void>
      visit_complement(const complement_t&)
      {
        raise(me(), ": cannot handle complement without generators");
      }

      /// Complement on a free labelset.
      template <bool IsFree>
      vcsn::enable_if_t<IsFree, void>
      visit_complement(const complement_t& e)
      {
        expansion_t res = to_expansion(e.sub());
        res_.constant = ws_.is_zero(res.constant) ? ws_.one() : ws_.zero();

        // Turn the polynomials into an expression, and complement it.
        for (auto l: rs_.labelset()->genset())
          ps_.add_here
            (res_.polynomials[l],
             polynomial_t{{rs_.complement(es_.as_expression(res.polynomials[l])),
                           ws_.one()}});
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
          res_.polynomials[p.first]
            = ps_.lmul(res_.constant,
                       ps_.rmul(p.second, f));
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

      /// Manipulate the expressions.
      expressionset_t rs_;
      /// Manipulate the labels.
      labelset_t ls_ = *rs_.labelset();
      /// Manipulate the weights.
      weightset_t ws_ = *rs_.weightset();
      /// Manipulate the polynomials of expressions.
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// Manipulate the expansions.
      expansionset_t es_ = expansionset_t(rs_);

      /// Whether to work transposed.
      bool transposed_ = false;
      /// The result.
      expansion_t res_;
    };

  } // rat::

  /// First order expansion.
  template <typename ExpSet>
  inline
  typename rat::expansionset<ExpSet>::value_t
  to_expansion(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    rat::to_expansion_visitor<ExpSet> to_expansion{rs};
    return to_expansion.to_expansion(e);
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
        const auto& rs = e.expressionset();
        auto es = vcsn::rat::expansionset<ExpSet>(rs);
        return make_expansion(es,
                              to_expansion<ExpSet>(rs, e.expression()));
      }

      REGISTER_DECLARE(to_expansion,
                       (const expression& e) -> expansion);
    }
  }
} // vcsn::

#endif // !VCSN_ALGOS_TO_EXPANSION_HH
