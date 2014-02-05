#ifndef VCSN_ALGOS_FIRST_ORDER_HH
# define VCSN_ALGOS_FIRST_ORDER_HH

# include <map>
# include <stack>
# include <unordered_map>

# include <vcsn/core/rat/visitor.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/indent.hh>
# include <vcsn/misc/map.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/unordered_set.hh>
# include <vcsn/weights/polynomialset.hh>

//# define DEBUG 1

namespace vcsn
{

  /*----------------------.
  | first_order(ratexp).  |
  `----------------------*/

  namespace rat
  {
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

      using polynomialset_t = ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;

      constexpr static const char* me() { return "first_order"; }

      // Keep it sorted to ensure determinism, and better looking
      // results.  Anyway, rough benches show no difference between
      // map and unordered_map here.
      using polys_t = std::map<label_t, polynomial_t, vcsn::less<labelset_t>>;

      /// Store the result.
      struct value_t
      {
        weight_t constant;
        polys_t polynomials;
      };

      first_order_visitor(const ratexpset_t& rs, bool use_spontaneous = false)
        : rs_(rs)
        , use_spontaneous_(use_spontaneous)
      {}

      void operator()(const ratexp_t& v)
      {
        // FIXME: make a libc++ bug report: "{ws_.zero(), {}}" should
        // suffice.
        res_ = {ws_.zero(), polys_t{}};
        v->accept(*this);
      }

      std::unordered_map<ratexp_t, value_t,
                         vcsn::hash<ratexpset_t>,
                         vcsn::equal_to<ratexpset_t>> cache_;

      value_t first_order(const ratexp_t& e)
      {
        auto insert = cache_.emplace(e, value_t{ws_.zero(), polys_t{}});
        auto& res = insert.first->second;
        if (insert.second)
          {
            std::swap(res, res_);
#if DEBUG
            rs_.print(std::cerr, e) << "..." << incendl;
#endif
            e->accept(*this);
            std::swap(res, res_);
#if DEBUG
            rs_.print(std::cerr, e) << " => ";
            print_(std::cerr, res) << decendl;
#endif
          }
        else
          {
#if DEBUG
            rs_.print(std::cerr, e) << " -> ";
            print_(std::cerr, res) << iendl;
#endif
          }
        return res;
      }

      polynomial_t first_order_as_polynomial(const ratexp_t& e)
      {
        operator()(e);
        return as_polynomial(res_);
      }

      polynomial_t as_polynomial(const value_t& v)
      {
        // FIXME: polynomial_t{{rs_.one(), constant}} is wrong,
        // because the (default) ctor will not eliminate the monomial
        // when constant is zero.
        polynomial_t res;
        ps_.add_weight(res, rs_.one(), v.constant);
        for (const auto& p: v.polynomials)
          // We may add a label on our maps, and later map it to 0.
          // In this case polynomialset builds '\z -> 1', i.e., it
          // does insert \z as a label in the polynomial.  Avoid this.
          //
          // FIXME: shouldn't polynomialset do that itself?
          if (!ps_.is_zero(p.second))
            ps_.add_weight(res,
                           rs_.mul(rs_.atom(p.first), ratexp_(p.second)),
                           ws_.one());
        return res;
      }

      // FIXME: duplicate with expand.
      ratexp_t ratexp_(const polynomial_t p)
      {
        ratexp_t res = rs_.zero();
        for (const auto& m: p)
          res = rs_.add(res, rs_.lmul(m.second, m.first));
         return res;
      }

      /// Print a first order development.
      std::ostream& print_(std::ostream& o, const value_t& v) const
      {
        ws_.print(o, v.constant);
        for (const auto& p: v.polynomials)
          {
            o << " + ";
            rs_.labelset()->print(o, p.first) << ".[";
            ps_.print(o, p.second) << ']';
          }
        return o;
      }

      /// In place addition.
      void add_(value_t& lhs, const value_t& rhs) const
      {
        lhs.constant = ws_.add(lhs.constant, rhs.constant);
        for (const auto& p: rhs.polynomials)
          ps_.add_weight(lhs.polynomials[p.first], p.second);
      }

      value_t& lmul_(const weight_t& w, value_t& res) const
      {
        res.constant = ws_.mul(w, res.constant);
        for (auto& p: res.polynomials)
          p.second = ps_.lmul(w, p.second);
        return res;
      }

      value_t& ldiv_(const weight_t& w, value_t& res) const
      {
        res.constant = ws_.ldiv(w, res.constant);
        for (auto& p: res.polynomials)
          for (auto& m: p.second)
            m.second = ws_.ldiv(w, m.second);
        return res;
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = {ws_.zero(), polys_t{}};
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = {ws_.one(), polys_t{}};
      }

      VCSN_RAT_VISIT(atom, e)
      {
        res_ = {ws_.zero(), {{e.value(), ps_.one()}}};
      }

      VCSN_RAT_VISIT(sum, e)
      {
        res_ = {ws_.zero(), polys_t{}};
        for (const auto& v: e)
          add_(res_, first_order(v));
      }

      VCSN_RAT_VISIT(prod, e)
      {
        res_ = {ws_.one(), polys_t{}};
        for (const auto& r: e)
          {
            // fo(l) = c(l) + a.A(l) + ...
            // fo(r) = c(r) + a.A(r) + ...
            // fo(l.r) = (c(l) + a.A(l) + ...) (c(r) + a.A(r) + ...)
            // c(fo(lr)) = c(l).c(r)
            // A(fo(lr)) = A(l).r + c(l).A(r)

            // (i): A(fo(lr)) = A(l).r
            for (auto& p: res_.polynomials)
              p.second = ps_.rmul(p.second, r);

            // Don't leave \z polynomials.
            if (!ws_.is_zero(res_.constant))
              {
                value_t rhs = first_order(r);
                // (ii) A(fo(lr)) += c(l).A(r)
                for (const auto& p: rhs.polynomials)
                  ps_.add_weight(res_.polynomials[p.first],
                                 ps_.lmul(res_.constant, p.second));

                // (iii) c(fo(lr)) = c(l).c(r)
                res_.constant = ws_.mul(res_.constant, rhs.constant);
              }
          }
      }

      label_t one_(std::true_type)
      {
        return rs_.labelset()->one();
      }

      label_t one_(std::false_type)
      {
        raise("first_order: quotient requires LAN");
      }

      VCSN_RAT_VISIT(ldiv, e)
      {
        assert(e.size() == 2);
#if DEBUG
        std::cerr << "Start: ";
        rs_.print(std::cerr, e.shared_from_this()) << " =>\n";
#endif
        // Special case the quotient of stars.
        if (!use_spontaneous_
            && e[0]->type() == type_t::star
            && e[1]->type() == type_t::star)
          {
            auto l = std::dynamic_pointer_cast<const star_t>(e[0]);
            auto r = std::dynamic_pointer_cast<const star_t>(e[1]);
            auto e = l->sub();
            auto f = r->sub();
            // (e*) \ (f*) = (e\f)* . f*
            auto q = rs_.mul(rs_.star(rs_.ldiv(e, f)), r);
            res_ = first_order(q);
          }
        else
          {
            value_t lhs = first_order(e[0]);
            value_t rhs = first_order(e[1]);
#if DEBUG
            std::cerr << "Lhs: "; print_(std::cerr, lhs) << '\n';
            std::cerr << "Rhs: "; print_(std::cerr, rhs) << '\n';
#endif
            res_ = {ws_.zero(), polys_t{}};
            // lp: (label, left_polynomial)
            if (!ws_.is_zero(lhs.constant))
              add_(res_, ldiv_(lhs.constant, rhs));
            /// FIXME: We really want some means to iterate on two
            /// maps simultaneously, which matching keys.  Explore
            /// this: <http://stackoverflow.com/questions/13840998>.
            for (const auto& lp: lhs.polynomials)
              {
                auto i = rhs.polynomials.find(lp.first);
                if (i != std::end(rhs.polynomials))
                  {
                    // rp: (label, right_polynomial).
                    const auto& rp = *i;
                    for (const auto& lm: lp.second)
                      for (const auto& rm: rp.second)
                        {
                          // Now, recursively develop the quotient of
                          // monomials, directly in res_.
                          auto q = rs_.ldiv(lm.first, rm.first);
                          if (use_spontaneous_)
                            {
                              static auto one =
                                one_(std::integral_constant<bool,
                                     context_t::is_lan>());
                              auto w = ws_.ldiv(lm.second, rm.second);
                              ps_.add_weight(res_.polynomials[one],
                                             q, w);
                            }
                          else
                            {
#if DEBUG
                              std::cerr << "Rec: (";
#endif
                              auto p = first_order(q);
#if DEBUG
                              print_(std::cerr, p) << '\n';
#endif

                              // (1/2)*2 is wrong in Z, (1*2)/2 is ok.
                              add_(res_, ldiv_(lm.second, lmul_(rm.second, p)));
                            }
                        }
                  }
              }
          }
      }

      VCSN_RAT_VISIT(intersection, e)
      {
        res_ = first_order(e.head());
        for (const auto& r: e.tail())
          {
            // Save current result in lhs, and compute the result in res.
            value_t lhs = {ws_.zero(), polys_t{}};
            std::swap(res_, lhs);
            value_t rhs = first_order(r);

            for (auto& p: lhs.polynomials)
              {
                auto i = rhs.polynomials.find(p.first);
                if (i != std::end(rhs.polynomials))
                  res_.polynomials[p.first]
                    = ps_.intersection(p.second, i->second);
              }
            res_.constant = ws_.mul(lhs.constant, rhs.constant);
          }
      }

      // FO(E:F) = FO(E):F + E:FO(F)
      VCSN_RAT_VISIT(shuffle, e)
      {
        value_t res = {ws_.one(), polys_t{}};
        // The shuffle-product of the previously traversed siblings.
        // Initially the neutral element: \e.
        ratexp_t prev = rs_.one();
        for (const auto& rhs: e)
          {
            // Save current result in lhs, and compute the result in res.
            value_t lhs; lhs.constant = ws_.zero();
            std::swap(res, lhs);

            value_t r = first_order(rhs);
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

      const labelset_t& letters_(std::true_type)
      {
        return *rs_.labelset();
      }

      std::vector<label_t> letters_(std::false_type)
      {
        raise(me(), ": cannot handle complement with generators");
      }

      VCSN_RAT_VISIT(complement, e)
      {
        value_t res = first_order(e.sub());
        res_.constant = ws_.is_zero(res.constant) ? ws_.one() : ws_.zero();

        // Turn the polynomials into a ratexp, and complement it.
        // Here, we need to iterate over the set of letters (obviously
        // required to complement).
        auto letters
          = letters_(std::integral_constant<bool,
                     context_t::is_lal || context_t::is_lan>());
        for (auto l: letters)
          ps_.add_weight
            (res_.polynomials[l],
             polynomial_t{{rs_.complement(ratexp_(res.polynomials[l])),
                           ws_.one()}});
      }

      VCSN_RAT_VISIT(star, e)
      {
        value_t res = first_order(e.sub());
        res_.constant = ws_.star(res.constant);
        for (const auto& p: res.polynomials)
          res_.polynomials[p.first]
            = ps_.lmul(res_.constant,
                       ps_.rmul(p.second, e.shared_from_this()));
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        value_t res = first_order(e.sub());
        res_.constant = ws_.mul(e.weight(), res.constant);
        for (const auto& p: res.polynomials)
          res_.polynomials[p.first] = ps_.lmul(e.weight(), p.second);
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        value_t res = first_order(e.sub());
        res_ = {ws_.mul(res.constant, e.weight()), polys_t{}};
        for (auto& p: res.polynomials)
          for (const auto& m: p.second)
            ps_.add_weight(res_.polynomials[p.first],
                           rs_.rmul(m.first, e.weight()), m.second);
      }

      // private:
      ratexpset_t rs_;
      /// Whether to use spontaneous transitions.
      bool use_spontaneous_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
      /// The result.
      value_t res_;
    };

  } // rat::

  /// First order expansion.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  first_order(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
              bool use_spontaneous = false)
  {
    rat::first_order_visitor<RatExpSet> first_order{rs, use_spontaneous};
    return first_order.first_order_as_polynomial(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Bool>
      polynomial
      first_order(const ratexp& exp, bool use_spontaneous = false)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.get_ratexpset();
        auto ps = vcsn::rat::make_ratexp_polynomialset(rs);
        return make_polynomial(ps,
                               first_order<RatExpSet>(rs, e.ratexp(),
                                                      use_spontaneous));
      }

      REGISTER_DECLARE(first_order,
                       (const ratexp& e, bool use_spontaneous) -> polynomial);
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
#if DEBUG
            std::cerr << "New state: ";
            rs_.print(std::cerr, r) << '\n';
#endif
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
            expand(r);
            res_.set_final(src, expand.res_.constant);
            for (const auto& p: expand.res_.polynomials)
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
        return make_automaton(linear(r.get_ratexpset(),
                                     r.ratexp(), use_spontaneous));
      }

      REGISTER_DECLARE(linear,
                       (const ratexp& e, bool use_spontaneous) -> automaton);
    }
  }
} // vcsn::

#endif // !VCSN_ALGOS_FIRST_ORDER_HH
