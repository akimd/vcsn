#ifndef VCSN_ALGOS_DERIVATION_HH
# define VCSN_ALGOS_DERIVATION_HH

# include <stack>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/algos/constant-term.hh>
# include <vcsn/algos/split.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/weights/polynomialset.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/polynomial.hh>

namespace vcsn
{

  /*---------------------.
  | derivation(ratexp).  |
  `---------------------*/

  namespace rat
  {
    template <typename RatExpSet>
    class derivation_visitor
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

      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;

      derivation_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      polynomial_t
      operator()(const ratexp_t& v, label_t var)
      {
        variable_ = var;
        v->accept(*this);
        return std::move(res_);
      }

      // FIXME: duplicate with expand.
      ratexp_t
      ratexp(const polynomial_t p)
      {
        ratexp_t res = rs_.zero();
        for (const auto& m: p)
          res = rs_.add(res, rs_.lmul(m.second, m.first));
         return res;
      }

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
            ps_.add_weight(res, res_);
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
            for (unsigned j = i + 1; j < n; ++j)
              res_ = ps_.rmul(res_, e[j]);
            ps_.add_weight(res, ps_.lmul(constant, res_));
            constant = ws_.mul(constant, constant_term(rs_, v));
          }
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(intersection, e)
      {
        // The first polynomial.
	e.head()->accept(*this);
        auto res = res_;
        for (unsigned i = 1, n = e.size(); i < n; ++i)
          {
            const auto& v = e[i];
            v->accept(*this);
            res = ps_.intersection(res, res_);
          }
        res_ = std::move(res);
      }

      /// Handle an n-ary shuffle.
      VCSN_RAT_VISIT(shuffle, e)
      {
        polynomial_t res = ps_.zero();
        for (unsigned i = 0; i < e.size(); ++i)
          {
            e[i]->accept(*this);
            for (const auto& m: res_)
              {
                typename node_t::ratexps_t ratexps;
                for (unsigned j = 0; j < e.size(); ++j)
                  if (i == j)
                    ratexps.emplace_back(m.first);
                  else
                    ratexps.emplace_back(e[j]);
                // FIXME: we need better n-ary constructors.
                ps_.add_weight(res,
                               std::make_shared<shuffle_t>(ratexps),
                               m.second);
              }
          }
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(complement, e)
      {
        e.sub()->accept(*this);
        // Turn the polynomial into a ratexp, and complement it.
        res_ = polynomial_t{{rs_.complement(ratexp(res_)), ws_.one()}};
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
        res_ = ps_.rmul(res_, e.weight());
      }

    private:
      ratexpset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
      /// The result.
      polynomial_t res_;
      /// The derivation variable.
      label_t variable_;
    };

  } // rat::

  /// Derive a ratexp wrt to a letter.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derivation(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
             typename RatExpSet::label_t a, bool breaking = false)
  {
    static_assert(RatExpSet::context_t::is_lal,
                  "requires labels_are_letters");
    rat::derivation_visitor<RatExpSet> derivation{rs};
    auto res = derivation(e, a);
    if (breaking)
      res = split(rs, res);
    return res;
  }


  /// Derive a polynomial of ratexps wrt to a letter.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derivation(const RatExpSet& rs,
             const rat::ratexp_polynomial_t<RatExpSet>& p,
             typename RatExpSet::label_t a, bool breaking = false)
  {
    auto ps = rat::make_ratexp_polynomialset(rs);
    using polynomial_t = rat::ratexp_polynomial_t<RatExpSet>;
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res,
                   ps.lmul(m.second, derivation(rs, m.first, a, breaking)));
    return res;
  }


  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derivation(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
             const std::string& s, bool breaking = false)
  {
    if (s.empty())
      throw std::runtime_error("cannot derivation wrt an empty string");
    auto res = derivation(rs, e, s[0], breaking);
    for (size_t i = 1, len = s.size(); i < len; ++i)
      res = derivation(rs, res, s[i], breaking);
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename String, typename Bool>
      polynomial
      derivation(const ratexp& exp, const std::string& s, bool breaking = false)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.get_ratexpset();
        auto ps = vcsn::rat::make_ratexp_polynomialset(rs);
        return make_polynomial(ps,
                               derivation<RatExpSet>(rs, e.ratexp(), s,
                                                     breaking));
      }

      REGISTER_DECLARE(derivation,
                       (const ratexp& e, const std::string& s,
                        bool breaking) -> polynomial);
    }
  }

  /*-----------------------.
  | derived_term(ratexp).  |
  `-----------------------*/

  namespace detail
  {
    template <typename RatExpSet>
    struct derived_termer
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

      struct ratexpset_less_than
      {
        bool operator()(const ratexp_t& lhs, const ratexp_t& rhs) const
        {
          return ratexpset_t::less_than(lhs, rhs);
        }
      };

      /// Symbolic states to state handlers.
      using smap = std::map<ratexp_t, state_t, ratexpset_less_than>;

      derived_termer(const ratexpset_t& rs, bool breaking = false)
        : rs_(rs)
        , breaking_(breaking)
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
            dst = res_.new_state();
            res_.set_final(dst, constant_term(rs_, r));
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
        // This is the labelset, but when iterated, the list of generators.
        const auto& ls = *rs_.labelset();

        // Turn the ratexp into a polynomial.
        {
          polynomial_t initial
            = breaking_ ? split(rs_, ratexp)
            : polynomial_t{{ratexp, ws.one()}};
          for (const auto& p: initial)
            // Also loads todo_ via state().
            res_.set_initial(state(p.first), p.second);
        }

        while (!todo_.empty())
          {
            ratexp_t r = todo_.top();
            todo_.pop();
            state_t src = map_[r];
            for (auto l : ls)
              for (const auto& m: derivation(rs_, r, l, breaking_))
                res_.add_transition(src, state(m.first), l, m.second);
          }
        return std::move(res_);
      }

      /// Map a state to its derived term.
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
            << str_escape(rs_.format(p.second))
            << "\"]\n";
        o << "*/\n";
        return o;
      }

    private:
      // List of states to visit.
      std::stack<ratexp_t> todo_;

      ratexpset_t rs_;
      smap map_;
      bool breaking_ = false;
      /// The resulting automaton.
      automaton_t res_;
    };
  }

  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  mutable_automaton<typename RatExpSet::context_t>
  derived_term(const RatExpSet& rs, const typename RatExpSet::ratexp_t& r,
               bool breaking = false)
  {
    detail::derived_termer<RatExpSet> dt{rs, breaking};
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
      derived_term(const ratexp& exp, bool breaking = false)
      {
        const auto& r = exp->as<RatExpSet>();
        return make_automaton(derived_term(r.get_ratexpset(),
                                           r.ratexp(), breaking));
      }

      REGISTER_DECLARE(derived_term,
                       (const ratexp& e, bool breaking) -> automaton);
    }
  }



} // vcsn::

#endif // !VCSN_ALGOS_DERIVATION_HH
