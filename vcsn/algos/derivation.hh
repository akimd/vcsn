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
  }


  /*----------------.
  | split(ratexp).  |
  `----------------*/

  namespace rat
  {
    template <typename RatExpSet>
    class split_visitor
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
      using prod_t = typename super_type::prod_t;
      using shuffle_t = typename super_type::shuffle_t;
      using intersection_t = typename super_type::intersection_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;
      using lweight_t = typename super_type::lweight_t;
      using rweight_t = typename super_type::rweight_t;

      split_visitor(const ratexpset_t& rs)
        : rs_(rs)
      {}

      /// Break a ratexp into a polynomial.
      polynomial_t operator()(const ratexp_t& v)
      {
        return split(v);
      }

      /// Easy recursion.
      polynomial_t split(const ratexp_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      virtual void
      visit(const zero_t&)
      {
        res_ = ps_.zero();
      }

      virtual void
      visit(const one_t&)
      {
        res_ = polynomial_t{{rs_.one(), ws_.one()}};
      }

      virtual void
      visit(const atom_t& e)
      {
        res_ = polynomial_t{{rs_.atom(e.value()), ws_.one()}};
      }

      virtual void
      visit(const sum_t& e)
      {
        polynomial_t res = ps_.zero();
        for (const auto& v: e)
          {
            v->accept(*this);
            ps_.add_weight(res, res_);
          }
        res_ = std::move(res);
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns split(l) x split(r).
      /// FIXME: This is inefficient, we split the lhs way too often.
      polynomial_t product(const ratexp_t& l, const ratexp_t& r)
      {
        // B(l).
        polynomial_t l_split = split(l);
        // constant-term(B(l)).
        weight_t l_split_const = ps_.get_weight(l_split, rs_.one());
        // proper(B(l)).
        ps_.del_weight(l_split, rs_.one());

        // res = proper(B(l)).r + constant-term(B(l))B(r).
        return ps_.add(ps_.rmul(l_split, r),
                       ps_.lmul(l_split_const, split(r)));
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns l x split(r).
      polynomial_t product(const polynomial_t& l, const ratexp_t& r)
      {
        polynomial_t res;
        for (const auto& m: l)
          ps_.add_weight(res, ps_.lmul(m.second, product(m.first, r)));
        return res;
      }

      /// Handle an n-ary product.
      virtual void
      visit(const prod_t& e)
      {
        auto res = product(e[0], e[1]);
        for (unsigned i = 2, n = e.size(); i < n; ++i)
          res = product(res, e[i]);
        res_ = std::move(res);
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns split(l) x split(r).
      /// FIXME: This is inefficient, we split the lhs way too often.
      polynomial_t intersection(const ratexp_t& l, const ratexp_t& r)
      {
        // B(l).
        polynomial_t l_split = split(l);
        // constant-term(B(l)).
        weight_t l_split_const = ps_.get_weight(l_split, rs_.one());
        // proper(B(l)).
        ps_.del_weight(l_split, rs_.one());

        // res = proper(B(l))&r.
        polynomial_t res;
        for (const auto& e: l_split)
          ps_.add_weight(res, rs_.intersection(e.first, r), e.second);
        // res += constant-term(B(l))B(r)
        ps_.add_weight(res,
                       ps_.lmul(l_split_const, split(r)));
        return res;
      }

      /// The split-product of \a l with \a r.
      ///
      /// Returns l x split(r).
      polynomial_t intersection(const polynomial_t& l, const ratexp_t& r)
      {
        polynomial_t res;
        for (const auto& m: l)
          ps_.add_weight(res, ps_.lmul(m.second, intersection(m.first, r)));
        return res;
      }

      /// Handle an n-ary intersection.
      virtual void
      visit(const intersection_t& e)
      {
        auto res = intersection(e[0], e[1]);
        for (unsigned i = 2, n = e.size(); i < n; ++i)
          res = intersection(res, e[i]);
        res_ = std::move(res);
      }

      virtual void
      visit(const shuffle_t&)
      {
        throw std::domain_error("split: shuffle is not supported");
      }

      using complement_t = typename super_type::complement_t;
      virtual void visit(const complement_t&)
      {
        throw std::domain_error("split: complement is not supported");
      }

      virtual void
      visit(const star_t& e)
      {
        // We need a copy of e, but without its weights.
        auto e2 = rs_.star(e.sub()->clone());
        res_ = polynomial_t{{e2, ws_.one()}};
      }

      virtual void
      visit(const lweight_t& e)
      {
        e.sub()->accept(*this);
        res_ = ps_.lmul(e.weight(), res_);
      }

      virtual void
      visit(const rweight_t& e)
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
    };
  }

  /// Split a ratexp.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  split(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e)
  {
    rat::split_visitor<RatExpSet> split{rs};
    return split(e);
  }

  /// Split a polynomial of ratexps.
  template <typename RatExpSet>
  inline
  rat::ratexp_polynomial_t<RatExpSet>
  split(const RatExpSet& rs, const rat::ratexp_polynomial_t<RatExpSet>& p)
  {
    auto ps = rat::make_ratexp_polynomialset(rs);
    using polynomial_t = rat::ratexp_polynomial_t<RatExpSet>;
    rat::split_visitor<RatExpSet> split{rs};
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res, ps.lmul(m.second, split(m.first)));
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      polynomial
      split(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.get_ratexpset();
        auto ps = vcsn::rat::make_ratexp_polynomialset(rs);
        return make_polynomial(ps,
                               split<RatExpSet>(rs, e.ratexp()));
      }

      REGISTER_DECLARE(split,
                       (const ratexp& e) -> polynomial);
    }
  }

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
      using shuffle_t = typename super_type::shuffle_t;
      using intersection_t = typename super_type::intersection_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;
      using lweight_t = typename super_type::lweight_t;
      using rweight_t = typename super_type::rweight_t;

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
          res_ = ps_.one();
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
            ps_.add_weight(res, res_);
          }
        res_ = std::move(res);
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
            ps_.add_weight(res, ps_.lmul(constant, res_));
            constant = ws_.mul(constant, constant_term(rs_, v));
          }
        res_ = std::move(res);
      }

      virtual void
      visit(const intersection_t& e)
      {
        // The first polynomial.
	e.head()->accept(*this);
        auto res = res_;
        for (unsigned i = 1, n = e.size(); i < n; ++i)
          {
            const auto& v = e[i];
            v->accept(*this);
            polynomial_t sum = ps_.zero();
            // Compute (l & r) where l and r are two polynomials:
            // distribute.
            for (const auto& l: res)
              for (const auto& r: res_)
                ps_.add_weight(sum,
                               rs_.intersection(l.first, r.first),
                               ws_.mul(l.second, r.second));
            res = sum;
          }
        res_ = std::move(res);
      }

      /// Handle an n-ary shuffle.
      virtual void
      visit(const shuffle_t& e)
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

      using complement_t = typename super_type::complement_t;
      virtual void
      visit(const complement_t& e)
      {
        e.sub()->accept(*this);
        // Turn the polynomial into a ratexp, and complement it.
        res_ = polynomial_t{{rs_.complement(ratexp(res_)), ws_.one()}};
      }

      virtual void
      visit(const star_t& e)
      {
        e.sub()->accept(*this);
        // We need a copy of e, but without its weights.
        auto e2 = rs_.star(e.sub()->clone());
        res_ = ps_.lmul(ws_.star(constant_term(rs_, e.sub())),
                        ps_.rmul(res_, e2));
      }

      virtual void
      visit(const lweight_t& e)
      {
        e.sub()->accept(*this);
        res_ = ps_.lmul(e.weight(), res_);
      }

      virtual void
      visit(const rweight_t& e)
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
             typename RatExpSet::label_t a)
  {
    static_assert(RatExpSet::context_t::is_lal,
                  "requires labels_are_letters");
    rat::derivation_visitor<RatExpSet> derivation{rs};
    auto res = derivation(e, a);
    if (getenv("VCSN_BREAKING"))
      res = split(rs, res);
    return res;
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
    polynomial_t res;
    for (const auto& m: p)
      res = ps.add(res, ps.lmul(m.second, derivation(rs, m.first, a)));
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
      /// Bridge.
      template <typename RatExpSet, typename String>
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

      /// Symbolic states: the derived terms are polynomials of ratexps.
      using polynomialset_t = rat::ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      struct ratexpset_less_than
      {
        const ratexpset_t& rs_;
        bool operator()(const ratexp_t& lhs, const ratexp_t& rhs) const
        {
          auto res = ratexpset_t::less_than(lhs, rhs);
          rs_.print(std::cerr, lhs) << " < ";
          rs_.print(std::cerr, rhs) << " = " << res << std::endl;
          return res;
        }
      };

      /// Symbolic states to state handlers.
      using smap = std::map<ratexp_t, state_t, ratexpset_less_than>;

      derived_termer(const ratexpset_t& rs)
        : rs_(rs)
        , map_(ratexpset_less_than{rs})
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
          polynomial_t initial;
          if (getenv("VCSN_BREAKING"))
            initial = split(rs_, ratexp);
          else
            initial = polynomial_t{{ratexp, ws.one()}};
          for (const auto& p: initial)
            {
              todo.push(p.first);
              state_t s = res.new_state();
              map_[p.first] = s;
              res.set_initial(s, p.second);
              res.set_final(s, constant_term(rs_, p.first));
            }
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
                        std::cerr << "New state: d/d" << l << "(";
                        rs_.print(std::cerr, r) << ") = ";
                        rs_.print(std::cerr, p.first) << std::endl;
                        res.set_final(dst, constant_term(rs_, p.first));
                        map_[p.first] = dst;
                        for (const auto& p2: map_)
                          rs_.print(std::cerr, p2.first) << ' ' << p2.second << std::endl;
                        std::cerr << std::endl;
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
    if (getenv("VCSN_ORIGINS"))
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
        return make_automaton(derived_term(r.get_ratexpset(),
                                           r.ratexp()));
      }

      REGISTER_DECLARE(derived_term,
                       (const ratexp& e) -> automaton);
    }
  }



} // vcsn::

#endif // !VCSN_ALGOS_DERIVATION_HH
