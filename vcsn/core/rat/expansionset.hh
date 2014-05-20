#ifndef VCSN_CORE_RAT_EXPANSIONSET_HH
# define VCSN_CORE_RAT_EXPANSIONSET_HH


namespace vcsn
{

  namespace rat
  {

    /*---------------.
    | expansionset.  |
    `---------------*/

    template <typename RatExpSet>
    struct expansionset
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = context_t_of<ratexpset_t>;
      using labelset_t = typename context_t::labelset_t;
      using label_t = typename context_t::label_t;
      using ratexp_t = typename ratexpset_t::value_t;
      using weightset_t = typename ratexpset_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using monomial_t = typename polynomialset_t::monomial_t;

      constexpr static const char* me() { return "expansion"; }

      // Keep it sorted to ensure determinism, and better looking
      // results.  Anyway, rough benches show no difference between
      // map and unordered_map here.
      using polys_t = std::map<label_t, polynomial_t, vcsn::less<labelset_t>>;

      /// An expansion.
      struct value_t
      {
        weight_t constant;
        polys_t polynomials;
      };

      expansionset(const ratexpset_t& rs)
        : rs_(rs)
      {}

      /// The static name.
      static std::string sname()
      {
        return "expansionset<" + ratexpset_t::sname() + ">";
      }

      /// The dynamic name.
      std::string vname(bool full = true) const
      {
        return "expansionset<" + rs_.vname(full) + ">";
      }

      /// Print a first order development.
      std::ostream& print(std::ostream& o, const value_t& v,
                          const std::string& format = "text") const
      {
        bool first = true;
        if (!ws_.is_zero(v.constant) || v.polynomials.empty())
          {
            o << (format == "latex" ? "\\langle " : "<");
            ws_.print(o, v.constant, format);
            o << (format == "latex" ? "\\rangle " : ">");
            first = false;
          }
        for (const auto& p: v.polynomials)
          {
            if (!first)
              o << (format == "latex" ? " \\oplus " : " + ");
            first = false;
            rs_.labelset()->print(o, p.first, format);
            o << (format == "latex" ? " \\odot \\left[" : ".[");;
            ps_.print(o, p.second, format);
            o << (format == "latex" ? "\\right]" : "]");;
          }
        return o;
      }

      /// The zero.
      value_t zero() const
      {
        return {ws_.zero(), polys_t{}};
      }

      /// The one.
      value_t one() const
      {
        return {ws_.one(), polys_t{}};
      }

      /// A single label.
      value_t atom(const label_t& l) const
      {
        return {ws_.zero(), {{l, ps_.one()}}};
      }

      /// In place addition.
      void add_here(value_t& lhs, const value_t& rhs) const
      {
        lhs.constant = ws_.add(lhs.constant, rhs.constant);
        for (const auto& p: rhs.polynomials)
          ps_.add_weight(lhs.polynomials[p.first], p.second);
      }

      /// Inplace left-multiplication by \a w of \a res.
      value_t& lmul_here(const weight_t& w, value_t& res) const
      {
        res.constant = ws_.mul(w, res.constant);
        for (auto& p: res.polynomials)
          p.second = ps_.lmul(w, p.second);
        return res;
      }

      /// Right-multiplication of \a lhs by \a w.
      value_t rmul(const value_t& lhs, const weight_t& w) const
      {
        value_t res = {ws_.mul(lhs.constant, w), polys_t{}};
        for (auto& p: lhs.polynomials)
          for (const auto& m: p.second)
            ps_.add_weight(res.polynomials[p.first],
                           rs_.rmul(m.first, w), m.second);
        return res;
      }

      /// In place right multiplication by a ratexp.
      value_t& rmul_here(value_t& res, const ratexp_t& rhs) const
      {
        for (auto& p: res.polynomials)
          p.second = ps_.rmul(p.second, rhs);
        return res;
      }

      /// Inplace left-division by \a w of \a res.
      value_t& ldiv_here(const weight_t& w, value_t& res) const
      {
        res.constant = ws_.ldiv(w, res.constant);
        for (auto& p: res.polynomials)
          for (auto& m: p.second)
            m.second = ws_.ldiv(w, m.second);
        return res;
      }

      /// The conjunction of \a l and \a r.
      value_t conjunction(const value_t& l, const value_t& r) const
      {
        value_t res = zero();
        res.constant = ws_.mul(l.constant, r.constant);
        for (const auto& p: zip_maps(l.polynomials, r.polynomials))
          res.polynomials[p.first] = ps_.conjunction(std::get<0>(p.second),
                                                     std::get<1>(p.second));
        return res;
      }

      /// Convert an expansion to a polynomial.
      polynomial_t as_polynomial(const value_t& v) const
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
                           rs_.mul(rs_.atom(p.first), as_ratexp(p.second)),
                           ws_.one());
        return res;
      }

      // FIXME: duplicate with expand.
      ratexp_t as_ratexp(const polynomial_t& p) const
      {
        ratexp_t res = rs_.zero();
        for (const auto& m: p)
          res = rs_.add(res, rs_.lmul(m.second, m.first));
         return res;
      }

    private:
      /// The ratexpset used for the expressions.
      ratexpset_t rs_;
      /// Shorthand to the weightset.
      weightset_t ws_ = *rs_.weightset();
      /// The polynomialset for the polynomials.
      polynomialset_t ps_ = make_ratexp_polynomialset(rs_);
    };
  }
}

#endif // !VCSN_CORE_RAT_EXPANSIONSET_HH
