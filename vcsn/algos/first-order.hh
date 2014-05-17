#ifndef VCSN_ALGOS_FIRST_ORDER_HH
# define VCSN_ALGOS_FIRST_ORDER_HH

# include <map>
# include <stack>
# include <unordered_map>

# include <vcsn/core/rat/visitor.hh>
# include <vcsn/core/rat/expansionset.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/expansion.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/indent.hh>
# include <vcsn/misc/map.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/unordered_set.hh>
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

    /*----------------------.
    | first_order_visitor.  |
    `----------------------*/

    template <typename RatExpSet>
    class first_order_visitor
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using labelset_t = typename context_t::labelset_t;
      using label_t = typename context_t::label_t;
      using ratexp_t = typename ratexpset_t::value_t;
      using weightset_t = typename ratexpset_t::weightset_t;
      using weight_t = typename weightset_t::value_t;
      using expansionset_t = expansionset<ratexpset_t>;

      using polynomialset_t = ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;

      constexpr static const char* me() { return "first_order"; }

      using polys_t = typename expansionset_t::polys_t;
      using expansion_t = typename expansionset_t::value_t;

      first_order_visitor(const ratexpset_t& rs, bool use_spontaneous = false)
        : rs_(rs)
        , use_spontaneous_(use_spontaneous)
      {}

      expansion_t operator()(const ratexp_t& v)
      {
        res_ = es_.zero();
        v->accept(*this);
        return res_;
      }

      std::unordered_map<ratexp_t, expansion_t,
                         vcsn::hash<ratexpset_t>,
                         vcsn::equal_to<ratexpset_t>> cache_;

      expansion_t first_order(const ratexp_t& e)
      {
#if CACHE
        auto insert = cache_.emplace(e, es_.zero());
        auto& res = insert.first->second;
        if (insert.second)
          {
            std::swap(res, res_);
            DEBUG_IF(rs_.print(std::cerr, e) << "..." << incendl);
            e->accept(*this);
            std::swap(res, res_);
            DEBUG_IF(
                     rs_.print(std::cerr, e) << " => ";
                     print_(std::cerr, res) << decendl;
                     );
          }
        else
          {
            DEBUG_IF(
                     rs_.print(std::cerr, e) << " -> ";
                     print_(std::cerr, res) << iendl;
                     );
          }
        return res;
#else
        auto res = es_.zero();
        std::swap(res, res_);
        e->accept(*this);
        std::swap(res, res_);
        DEBUG_IF(
                 rs_.print(std::cerr, e) << " -> ";
                 print_(std::cerr, res) << iendl;
                 );
        return res;
#endif
      }

      polynomial_t first_order_as_polynomial(const ratexp_t& e)
      {
        operator()(e);
        return es_.as_polynomial(res_);
      }

      /// Print an expansion.
      std::ostream& print_(std::ostream& o, const expansion_t& v) const
      {
        es_.print(o, v);
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
        ls_.print(std::cerr, e.value())
          << " " << transposed_
          << " => ";
        res_ = es_.atom(transposed_
                        ? ls_.transpose(e.value())
                        : e.value());
        es_.print(std::cerr, res_) << '\n';
      }

      VCSN_RAT_VISIT(sum, e)
      {
        res_ = es_.zero();
        for (const auto& v: e)
          es_.add_here(res_, first_order(v));
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
              // linear([ab]*a[ab]{150}) from 0.32s to 0.23s on
              // erebus, clang++ 3.4.
              ratexp_t rhss
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
              expansion_t rhs = first_order(r);
              if (transposed_)
                r = rs_.transposition(r);

              es_.rmul_here(res_, r);

              for (const auto& p: rhs.polynomials)
                ps_.add_weight(res_.polynomials[p.first],
                               ps_.lmul(res_.constant, p.second));
              res_.constant = ws_.mul(res_.constant, rhs.constant);
            }
      }

      /// Build a product for these expressions.  Pay attention to not
      /// building products with 0 or 1 expression.
      ratexp_t
      prod_(typename prod_t::iterator begin,
            typename prod_t::iterator end) const
      {
        using ratexps_t = typename prod_t::ratexps_t;
        if (begin == end)
          return rs_.one();
        else if (std::next(begin, 1) == end)
          return *begin;
        else
          return std::make_shared<prod_t>(ratexps_t{begin, end});
      }

      label_t one_(std::true_type)
      {
        return rs_.labelset()->one();
      }

      label_t one_(std::false_type)
      {
        raise(me(), ": quotient requires LAN");
      }

      /// If r is e*, return e.
      /// If r is e*{T}, return e{T}.
      /// Otherwise return nullptr.
      ///
      /// FIXME: What about complement?
      ratexp_t star_child(const ratexp_t r)
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
                 rs_.print(std::cerr, e.shared_from_this()) << " =>\n";
                 );
        if (use_spontaneous_)
          visit_ldiv_with_one(e);
        else
          visit_ldiv_without_one(e);
      }

      void visit_ldiv_with_one(const ldiv_t& e)
      {
        bool transposed = transposed_;
        transposed_ = false;
        expansion_t lhs = first_order(e[0]);
        expansion_t rhs = first_order(e[1]);
        transposed_ = transposed;
        DEBUG_IF(
                 std::cerr << "Lhs: "; print_(std::cerr, lhs) << '\n';
                 std::cerr << "Rhs: "; print_(std::cerr, rhs) << '\n';
                 );
        res_ = es_.zero();
        // lp: (label, left_polynomial)
        if (!ws_.is_zero(lhs.constant))
          {
            if (transposed_)
              {
                auto rhs_transposed = first_order(e[1]);
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
              ps_.add_weight(res_.polynomials[one],
                             rs_.ldiv(lm.first, rm.first),
                             ws_.ldiv(lm.second, rm.second));
      }

      void visit_ldiv_without_one(const ldiv_t& e)
      {
        // Special case the quotient of stars.
        ratexp_t lchild = star_child(e[0]);
        ratexp_t rchild = star_child(e[1]);
        if (lchild && rchild)
          {
            // (e*) \ (f*) = (e\f)* . f*
            auto q = rs_.mul(rs_.star(rs_.ldiv(lchild, rchild)),
                             rs_.star(rchild));
            res_ = first_order(q);
          }
        else
          {
            bool transposed = transposed_;
            transposed_ = false;
            expansion_t lhs = first_order(e[0]);
            expansion_t rhs = first_order(e[1]);
            transposed_ = transposed;
            DEBUG_IF(
                     std::cerr << "Lhs: "; print_(std::cerr, lhs) << '\n';
                     std::cerr << "Rhs: "; print_(std::cerr, rhs) << '\n';
                     );
            res_ = es_.zero();
            // lp: (label, left_polynomial)
            if (!ws_.is_zero(lhs.constant))
              {
                if (transposed_)
                  {
                    auto rhs_transposed = first_order(e[1]);
                    es_.add_here(res_,
                                 es_.ldiv_here(lhs.constant, rhs_transposed));
                  }
                else
                  es_.add_here(res_, es_.ldiv_here(lhs.constant, rhs));
              }
            for (const auto& p: zip_maps(lhs.polynomials, rhs.polynomials))
              for (const auto& lm: std::get<0>(p.second))
                for (const auto& rm: std::get<1>(p.second))
                  {
                    // Now, recursively develop the quotient of
                    // monomials, directly in res_.
                    auto q = rs_.ldiv(lm.first, rm.first);
                    DEBUG_IF(std::cerr << "Rec: (");
                    auto p = first_order(q);
                    DEBUG_IF(print_(std::cerr, p) << '\n');
                    // (1/2)*2 is wrong in Z, (1*2)/2 is ok.
                    es_.add_here(res_,
                                 es_.ldiv_here(lm.second,
                                               es_.lmul_here(rm.second, p)));
                  }
          }
      }

      VCSN_RAT_VISIT(conjunction, e)
      {
        res_ = first_order(e.head());
        for (const auto& r: e.tail())
          res_ = es_.conjunction(res_, first_order(r));
      }

      // FO(E:F) = FO(E):F + E:FO(F)
      VCSN_RAT_VISIT(shuffle, e)
      {
        expansion_t res = es_.one();
        // The shuffle-product of the previously traversed siblings.
        // Initially the neutral element: \e.
        ratexp_t prev = rs_.one();
        for (const auto& rhs: e)
          {
            // Save current result in lhs, and compute the result in res.
            expansion_t lhs; lhs.constant = ws_.zero();
            std::swap(res, lhs);

            expansion_t r = first_order(rhs);
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
                ps_.add_weight(res.polynomials[p.first],
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
      typename std::enable_if<!IsFree, void>::type
      visit_complement(const complement_t&)
      {
        raise(me(), ": cannot handle complement without generators");
      }

      /// Complement on a free labelset.
      template <bool IsFree>
      typename std::enable_if<IsFree, void>::type
      visit_complement(const complement_t& e)
      {
        expansion_t res = first_order(e.sub());
        res_.constant = ws_.is_zero(res.constant) ? ws_.one() : ws_.zero();

        // Turn the polynomials into a ratexp, and complement it.
        for (auto l: rs_.labelset()->genset())
          ps_.add_weight
            (res_.polynomials[l],
             polynomial_t{{rs_.complement(es_.as_ratexp(res.polynomials[l])),
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
        expansion_t res = first_order(e.sub());
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
        auto r = first_order(e.sub());
        res_
          = transposed_
          ? es_.rmul(r, ws_.transpose(l))
          : es_.lmul_here(l, r);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        auto l = first_order(e.sub());
        auto r = e.weight();
        res_
          = transposed_
          ? es_.lmul_here(ws_.transpose(r), l)
          : es_.rmul(l, r);
      }

      /// Manipulate the ratexps.
      ratexpset_t rs_;
      /// Manipulate the labels.
      labelset_t ls_ = *rs_.labelset();
      /// Manipulate the weights.
      weightset_t ws_ = *rs_.weightset();
      /// Manipulate the polynomials of ratexps.
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
      /// Manipulate the expansions.
      expansionset_t es_ = expansionset_t(rs_);

      /// Whether to use spontaneous transitions.
      bool use_spontaneous_;
      /// Whether to work transposed.
      bool transposed_ = false;
      /// The result.
      expansion_t res_;
    };

  } // rat::

  /// First order expansion.
  template <typename RatExpSet>
  inline
  typename rat::expansionset<RatExpSet>::value_t
  first_order(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
              bool use_spontaneous = false)
  {
    rat::first_order_visitor<RatExpSet> first_order{rs, use_spontaneous};
    return first_order.first_order(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Bool>
      expansion
      first_order(const ratexp& exp, bool use_spontaneous = false)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.ratexpset();
        auto es = vcsn::rat::expansionset<RatExpSet>(rs);
        return make_expansion(es,
                              first_order<RatExpSet>(rs, e.ratexp(),
                                                     use_spontaneous));
      }

      REGISTER_DECLARE(first_order,
                       (const ratexp& e, bool use_spontaneous) -> expansion);
    }
  }

  /*-----------------.
  | linear(ratexp).  |
  `-----------------*/

  namespace detail
  {
    template <typename RatExpSet>
    struct linearer
    {
      using ratexpset_t = RatExpSet;
      using ratexp_t = typename ratexpset_t::value_t;

      using context_t = typename ratexpset_t::context_t;
      using labelset_t = typename context_t::labelset_t;
      using label_t = typename labelset_t::value_t;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;

      using automaton_t = mutable_automaton<context_t>;
      using state_t = typename automaton_t::state_t;

      /// Symbolic states: the derived terms are polynomials of ratexps.
      using polynomialset_t = rat::ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      /// Symbolic states to state handlers.
      using smap = std::unordered_map<ratexp_t, state_t,
                                      vcsn::hash<ratexpset_t>,
                                      vcsn::equal_to<ratexpset_t>>;

      linearer(const ratexpset_t& rs, bool use_spontaneous = false)
        : rs_(rs)
        , use_spontaneous_(use_spontaneous)
        , res_{rs_.context()}
      {}

      /// The state for ratexp \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const ratexp_t& r)
      {
        state_t dst;
        auto i = map_.find(r);
        if (i == end(map_))
          {
            DEBUG_IF(
                     std::cerr << "New state: ";
                     rs_.print(std::cerr, r) << '\n';
                     );
            dst = res_.new_state();
            map_[r] = dst;
            todo_.push(r);
          }
        else
          dst = i->second;
        return dst;
      }

      automaton_t operator()(const ratexp_t& ratexp)
      {
        weightset_t ws = *rs_.weightset();
        // Turn the ratexp into a polynomial.
        {
          polynomial_t initial
            = breaking_ ? split(rs_, ratexp)
            : polynomial_t{{ratexp, ws.one()}};
          for (const auto& p: initial)
            // Also loads todo_ via state().
            res_.set_initial(state(p.first), p.second);
        }
        rat::first_order_visitor<ratexpset_t> expand{rs_, use_spontaneous_};

        while (!todo_.empty())
          {
            ratexp_t r = todo_.top();
            todo_.pop();
            state_t src = map_[r];
            auto expansion = expand(r);
            res_.set_final(src, expansion.constant);
            for (const auto& p: expansion.polynomials)
              for (const auto& m: p.second)
                res_.add_transition(src, state(m.first), p.first, m.second);
          }
        return std::move(res_);
      }

      /// Ordered map: state -> its derived term.
      using origins_t = std::map<state_t, ratexp_t>;
      origins_t
      origins() const
      {
        origins_t res;
        for (const auto& p: map_)
          res[p.second] = p.first;
        return res;
      }

      /// Print the origins.
      std::ostream&
      print(std::ostream& o, const origins_t& orig) const
      {
        o << "/* Origins.\n"
             "    node [shape = box, style = rounded]\n";
        for (auto p : orig)
          o << "    " << p.first - 2
            << " [label = \""
            << str_escape(format(rs_, p.second))
            << "\"]\n";
        o << "*/\n";
        return o;
      }

    private:
      /// States to visit.
      std::stack<ratexp_t> todo_;
      /// The ratexp's set.
      ratexpset_t rs_;
      /// ratexp -> state.
      smap map_;
      bool use_spontaneous_ = false;
      /// Whether to perform a breaking derivation.
      bool breaking_ = false;
      /// The resulting automaton.
      automaton_t res_;
    };
  }

  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  mutable_automaton<typename RatExpSet::context_t>
  linear(const RatExpSet& rs, const typename RatExpSet::ratexp_t& r,
         bool use_spontaneous = false)
  {
    detail::linearer<RatExpSet> dt{rs, use_spontaneous};
    auto res = dt(r);
    if (getenv("VCSN_ORIGINS"))
      dt.print(std::cout, dt.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Book>
      automaton
      linear(const ratexp& exp, bool use_spontaneous = false)
      {
        const auto& r = exp->as<RatExpSet>();
        return make_automaton(linear(r.ratexpset(),
                                     r.ratexp(), use_spontaneous));
      }

      REGISTER_DECLARE(linear,
                       (const ratexp& e, bool use_spontaneous) -> automaton);
    }
  }
} // vcsn::

#endif // !VCSN_ALGOS_FIRST_ORDER_HH
