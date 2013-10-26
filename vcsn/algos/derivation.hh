#ifndef VCSN_ALGOS_DERIVATION_HH
# define VCSN_ALGOS_DERIVATION_HH

# include <set>
# include <stack>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/algos/constant-term.hh>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/weights/polynomialset.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/polynomial.hh>

namespace vcsn
{

  namespace rat
  {
    // FIXME: this is a general feature which is useful elsewhere.
    // E.g., expand.

    /// Type of PolynomialSet of RatExps from the RatExpSet type.
    template <typename RatExpSet>
    using ratexp_polynomialset_t
      = polynomialset<ctx::context<RatExpSet,
                                   typename RatExpSet::weightset_t>>;

    /// Type of polynomials of ratexps from the RatExpSet type.
    template <typename RatExpSet>
    using ratexp_polynomial_t
      = typename ratexp_polynomialset_t<RatExpSet>::value_t;

    /// From a RatExpSet to its polynomialset.
    template <typename RatExpSet>
    inline
    ratexp_polynomialset_t<RatExpSet>
    make_ratexp_polynomialset(const RatExpSet& rs)
    {
      using context_t = ctx::context<RatExpSet,
                                     typename RatExpSet::weightset_t>;
      return context_t{rs, *rs.weightset()};
    }


    /*---------------------.
    | derivation(ratexp).  |
    `---------------------*/

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
      using nary_t = typename super_type::nary_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

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


      void
      apply_weights(const inner_t& e)
      {
        res_ = ps_.lmul(e.left_weight(), res_);
        res_ = ps_.rmul(res_, e.right_weight());
      }

      void
      apply_weights(const leaf_t& e)
      {
        res_ = ps_.lmul(e.left_weight(), res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = ps_.zero();
      }

      virtual void
      visit(const one_t&)
      {
        res_ = ps_.zero();
      }

      virtual void
      visit(const atom_t& e)
      {
        if (e.value() == variable_)
          {
            res_ = ps_.one();
            apply_weights(e);
          }
        else
          res_ = ps_.zero();
      }

      virtual void
      visit(const sum_t& e)
      {
        polynomial_t res = ps_.zero();
        for (const auto& v: e)
          {
            v->accept(*this);
            res = ps_.add(res, res_);
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const prod_t& e)
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
            res = ps_.add(res, ps_.lmul(constant, res_));
            constant = ws_.mul(constant, constant_term(rs_, v));
          }
        res_ = res;
        apply_weights(e);
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        // We need a copy of e, but without its weights.
        auto e2 = rs_.star(e.sub()->clone());
        res_ = ps_.lmul(ws_.star(constant_term(rs_, e.sub())),
                        ps_.rmul(res_, e2));
        apply_weights(e);
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
             typename RatExpSet::label_t a)
  {
    static_assert(RatExpSet::context_t::is_lal,
                  "requires labels_are_letters");
    rat::derivation_visitor<RatExpSet> derivation{rs};
    return derivation(e, a);
  }


  /// Derive a polynomial of ratexps wrt to a letter.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derivation(const RatExpSet& rs,
             const rat::ratexp_polynomial_t<RatExpSet>& p,
             typename RatExpSet::label_t a)
  {
    auto ps = rat::make_ratexp_polynomialset(rs);
    using polynomial_t = rat::ratexp_polynomial_t<RatExpSet>;
    rat::derivation_visitor<RatExpSet> derivation{rs};
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res, ps.lmul(m.second, derivation(m.first, a)));
    return res;
  }


  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  derivation(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
             const std::string& s)
  {
    if (s.empty())
      throw std::runtime_error("cannot derivation wrt an empty string");
    auto res = derivation(rs, e, s[0]);
    for (size_t i = 1, len = s.size(); i < len; ++i)
      res = derivation(rs, res, s[i]);
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /*-------------------------------.
      | dyn::derivation(exp, string).  |
      `-------------------------------*/
      template <typename RatExpSet>
      polynomial
      derivation(const ratexp& exp, const std::string& s)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.get_ratexpset();
        auto ps = vcsn::rat::make_ratexp_polynomialset(rs);
        return make_polynomial(ps,
                               derivation<RatExpSet>(rs, e.ratexp(), s));
      }

      REGISTER_DECLARE(derivation,
                       (const ratexp& e, const std::string& s) -> polynomial);
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

      /// Symbolic states: the derived term are polynomials of ratexps.
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

      derived_termer(const ratexpset_t& rs)
        : rs_(rs)
      {}

      automaton_t operator()(const ratexp_t& ratexp)
      {
        weightset_t ws = *rs_.weightset();
        // This is the labelset, but when iterated, the list of generators.
        const auto& ls = *rs_.labelset();

        automaton_t res{rs_.context()};

        /// List of states to visit.
        std::stack<ratexp_t> todo;
        /// Turn the ratexp into a polynomial.
        {
          todo.push(ratexp);
          state_t s = res.new_state();
          map_[ratexp] = s;
          res.set_initial(s);
          res.set_final(s, constant_term(rs_, ratexp));
        }
        while (!todo.empty())
          {
            ratexp_t r = todo.top();
            todo.pop();
            state_t src = map_[r];
            for (auto l : ls)
              {
                polynomial_t next = derivation(rs_, r, l);
                for (const auto& p: next)
                  {
                    state_t dst;
                    auto i = map_.find(p.first);
                    if (i == end(map_))
                      {
                        dst = res.new_state();
                        res.set_final(dst, constant_term(rs_, p.first));
                        map_[p.first] = dst;
                        todo.push(p.first);
                      }
                    else
                      dst = i->second;
                    res.add_transition(src, dst, l, p.second);
                  }
              }
          }
        return std::move(res);
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
        o << "/* Origins." << std::endl;
        for (auto p : orig)
          {
            o << "    " << p.first - 2
              << " [label = \"";
            rs_.print(o, p.second);
            o << "\"]" << std::endl;
          }
        o << "*/" << std::endl;
        return o;
      }

    private:
      ratexpset_t rs_;
      smap map_;
    };
  }

  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  mutable_automaton<typename RatExpSet::context_t>
  derived_term(const RatExpSet& rs, const typename RatExpSet::ratexp_t& r)
  {
    detail::derived_termer<RatExpSet> dt{rs};
    auto res = dt(r);
    if (getenv("VCSN_DERIVED_TERMS"))
      dt.print(std::cout, dt.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      automaton
      derived_term(const ratexp& exp)
      {
        const auto& r = exp->as<RatExpSet>();
        return make_automaton(r.get_ratexpset().context(),
                              derived_term(r.get_ratexpset(),
                                            r.ratexp()));
      }

      REGISTER_DECLARE(derived_term,
                       (const ratexp& e) -> automaton);
    }
  }



} // vcsn::

#endif // !VCSN_ALGOS_DERIVATION_HH
