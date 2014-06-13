#ifndef VCSN_CORE_RAT_EXPANSIONSET_HH
# define VCSN_CORE_RAT_EXPANSIONSET_HH

# include <vcsn/algos/split.hh> // ratexp_polynomialset_t.
# include <vcsn/misc/map.hh>

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
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using ratexp_t = typename ratexpset_t::value_t;
      using weightset_t = weightset_t_of<ratexpset_t>;
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
      std::ostream& print(const value_t& v, std::ostream& o,
                          const std::string& format = "text") const
      {
        bool first = true;
        if (!ws_.is_zero(v.constant) || v.polynomials.empty())
          {
            o << (format == "latex" ? "\\langle " : "<");
            ws_.print(v.constant, o, format);
            o << (format == "latex" ? "\\rangle " : ">");
            first = false;
          }
        for (const auto& p: v.polynomials)
          {
            if (!first)
              o << (format == "latex" ? " \\oplus " : " + ");
            first = false;
            rs_.labelset()->print(p.first, o, format);
            o << (format == "latex" ? " \\odot \\left[" : ".[");;
            ps_.print(p.second, o, format);
            o << (format == "latex" ? "\\right]" : "]");;
          }
        return o;
      }

      /// Normalize \a res:
      /// There must not remain a constant-term associated to one:
      /// put it with the constant term of the expansion.
      value_t& normalize_(value_t& res, std::true_type) const
      {
        auto one = rs_.labelset()->one();
        auto i = res.polynomials.find(one);
        if (i != std::end(res.polynomials))
          {
            auto j = i->second.find(rs_.one());
            if (j != std::end(i->second))
              {
                res.constant = ws_.add(res.constant, j->second);
                i->second.erase(j);
                if (i->second.empty())
                  res.polynomials.erase(i);
              }
          }
        return res;
      }

      value_t& normalize_(value_t& res, std::false_type) const
      {
        return res;
      }

      value_t& normalize(value_t& res) const
      {
        auto has_one = std::integral_constant<bool, context_t::has_one()>();
        return normalize_(res, has_one);
      }

      /// Denormalize \a res move the constant to the polynomial
      /// associated to one.
      value_t& denormalize_(value_t& res, std::true_type) const
      {
        auto one = rs_.labelset()->one();
        if (!ws_.is_zero(res.constant))
          {
            ps_.add_weight(res.polynomials[one],
                           polynomial_t{{rs_.one(), res.constant}});
            res.constant = ws_.zero();
          }
        return res;
      }

      value_t& denormalize_(value_t& res, std::false_type) const
      {
        return res;
      }

      value_t& denormalize(value_t& res) const
      {
        auto has_one = std::integral_constant<bool, context_t::has_one()>();
        return denormalize_(res, has_one);
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
        return normalize(res);
      }

      void
      conjunctions_with_one_(value_t&,
                             const value_t&, const value_t&,
                             std::false_type) const
      {}

      void
      conjunctions_with_one_(value_t& res,
                             const value_t& l, const value_t& r,
                             std::true_type) const
      {
        // Spontaneous transitions from the lhs.
        auto one = rs_.labelset()->one();
        {
          auto i = l.polynomials.find(one);
          if (i != std::end(l.polynomials))
            for (const auto& rhs: r.polynomials)
              if (!rs_.labelset()->is_one(rhs.first))
                ps_.add_weight(res.polynomials[one],
                               ps_.conjunction(i->second,
                                               ps_.lmul(rs_.atom(rhs.first),
                                                        rhs.second)));
        }
        // Spontaneous transitions from the rhs.
        {
          auto i = r.polynomials.find(one);
          if (i != std::end(r.polynomials))
            for (const auto& lhs: l.polynomials)
              if (!rs_.labelset()->is_one(lhs.first))
                ps_.add_weight(res.polynomials[one],
                               ps_.conjunction(ps_.lmul(rs_.atom(lhs.first),
                                                        lhs.second),
                                               i->second));
        }
        normalize(res);
      }

      /// The conjunction of \a l and \a r.
      value_t conjunction(value_t l, value_t r) const
      {
        value_t res = zero();
        denormalize(l);
        denormalize(r);
        res.constant = ws_.mul(l.constant, r.constant);
        for (const auto& p: zip_maps(l.polynomials, r.polynomials))
          res.polynomials[p.first]
            = ps_.conjunction(std::get<0>(p.second), std::get<1>(p.second));

        auto has_one = std::integral_constant<bool, context_t::has_one()>();
        conjunctions_with_one_(res, l, r, has_one);
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
