#pragma once

#include <vcsn/algos/project.hh>
#include <vcsn/algos/split.hh> // expression_polynomialset_t
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/algorithm.hh>
#include <vcsn/misc/map.hh>

namespace vcsn
{

  namespace rat
  {

    /*---------------.
    | expansionset.  |
    `---------------*/

    template <typename ExpSet>
    struct expansionset
    {
    public:
      using expressionset_t = ExpSet;
      using self_t = expansionset;
      using context_t = context_t_of<expressionset_t>;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using expression_t = typename expressionset_t::value_t;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = typename weightset_t::value_t;

      using polynomialset_t = expression_polynomialset_t<expressionset_t>;
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
        // Default value to silence the warnings from
        // -Wmissing-field-initializers with `value_t{weight}`.
        polys_t polynomials = {};
      };

      expansionset(const expressionset_t& rs)
        : rs_(rs)
      {}

      /// The static name.
      static symbol sname()
      {
        static auto res
          = symbol{"expansionset<" + expressionset_t::sname() + '>'};
        return res;
      }

      /// The expressionset.
      const expressionset_t& expressionset() const
      {
        return rs_;
      }

      /// The polynomialset.
      const polynomialset_t& polynomialset() const
      {
        return ps_;
      }

      /// The context.
      const context_t& context() const
      {
        return rs_.context();
      }

      /// Print this valueset.
      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        switch (fmt.kind())
          {
          case format::latex:
            o << "\\mathsf{Expansion}[";
            rs_.print_set(o, fmt);
            o << ']';
            break;
          case format::sname:
            o << "expansionset<";
            rs_.print_set(o, fmt);
            o << ">";
            break;
          case format::text:
          case format::utf8:
            o << "Expansion[";
            rs_.print_set(o, fmt);
            o << ']';
            break;
          case format::ere:
          case format::raw:
          case format::redgrep:
            assert(0);
            break;
          }
        return o;
      }

      /// Print this expansion.
      std::ostream& print(const value_t& v,
                          std::ostream& o = std::cout,
                          format fmt = {}) const
      {
        bool first = true;
        if (!ws_.is_zero(v.constant) || v.polynomials.empty())
          {
            o << (fmt == format::latex ? "\\left\\langle "
                  : fmt == format::utf8 ? "⟨"
                  : "<");
            ws_.print(v.constant, o, fmt.for_weights());
            o << (fmt == format::latex ? "\\right\\rangle "
                  : fmt == format::utf8 ? "⟩"
                  : ">");
            first = false;
          }
        for (const auto& p: v.polynomials)
          {
            if (!first)
              o << (fmt == format::latex ? " \\oplus "
                    : fmt == format::utf8 ? " ⊕ "
                    : " + ");
            first = false;
            ls_.print(p.first, o, fmt.for_labels());
            o << (fmt == format::latex ? " \\odot \\left["
                  : fmt == format::utf8 ? "⊙["
                  : ".[");
            ps_.print(p.second, o, fmt);
            o << (fmt == format::latex ? "\\right]"
                  : fmt == format::utf8 ? "]"
                  : "]");
          }
        return o;
      }

      /// Whether an expansion is normal.
      bool is_normal(const value_t& x) const
      {
        return !has(x.polynomials, ls_.one());
      }

      /// Normalize: eliminate null polynomials and move the constant
      /// term from the label one.
      value_t& normalize(value_t& res) const
      {
        detail::erase_if(res.polynomials,
                         [this](auto& p)
                         {
                           return ps_.is_zero(p.second);
                         });
        return normalize_(res);
      }

      /// Normalize \a res.
      ///
      /// There must not remain a constant-term associated to one: put
      /// it with the constant term of the expansion.  I.e., turn
      /// `ε⊙[⟨w⟩ε + P...] + X_p...` into `⟨w⟩ + ε⊙[P...] + X_p...`.
      value_t& normalize_(value_t& res) const
      {
        auto one = ls_.one();
        auto i = res.polynomials.find(one);
        if (i != std::end(res.polynomials))
          {
            auto j = i->second.find(rs_.one());
            if (j != std::end(i->second))
              {
                res.constant = ws_.add(res.constant, weight_of(*j));
                i->second.erase(j);
                if (ps_.is_zero(i->second))
                  res.polynomials.erase(i);
              }
          }
        return res;
      }

      /// Move the constant to the polynomial associated to one.
      ///
      /// Note that this should be named `maybe_denormalize`, as if
      /// the labelset has no one, the input argument is returned
      /// unmodified.  In particular, be sure to use the constant term
      /// of the result, even if in typically cases it is expected to
      /// be zero.
      value_t& denormalize(value_t& res) const
      {
        return denormalize_(res);
      }

      /// Denormalize \a res move the constant to the polynomial
      /// associated to one.
      value_t& denormalize_(value_t& res) const
      {
        if (!ws_.is_zero(res.constant))
          {
            auto one = ls_.one();
            ps_.add_here(res.polynomials[one],
                         polynomial_t{{rs_.one(), res.constant}});
            res.constant = ws_.zero();
          }
        return res;
      }

      /*--------.
      | Conv.   |
      `--------*/

      /// Conversion from (this and) other weightsets.
      static value_t
      conv(self_t, const value_t& v)
      {
        return v;
      }

      /// Convert from another expansionset to self.
      template <typename OtherExpSet>
      value_t
      conv(const expansionset<OtherExpSet>& other,
           const typename expansionset<OtherExpSet>::value_t& v) const
      {
        const auto& other_ws = other.expressionset().weightset();
        const auto& other_ps = other.polynomialset();
        return {ws_.conv(other_ws, v.constant),
                ps_.conv(other_ps, v.polynomials)};
      }

      /// The zero.
      value_t zero() const
      {
        return {ws_.zero()};
      }

      /// The one.
      value_t one() const
      {
        return {ws_.one()};
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
          ps_.add_here(lhs.polynomials[p.first], p.second);
        normalize(lhs);
      }

      /// Addition.
      value_t add(value_t res, const value_t& rhs) const
      {
        add_here(res, rhs);
        return res;
      }

      /// Left-multiplication by \a w of \a rhs.
      value_t lweight(const weight_t& w, value_t res) const
      {
        lweight_here(w, res);
        return res;
      }

      /// Inplace left-multiplication by \a w of \a res.
      value_t& lweight_here(const weight_t& w, value_t& res) const
      {
        if (ws_.is_zero(w))
          res = zero();
        else
          {
            res.constant = ws_.mul(w, res.constant);
            for (auto& p: res.polynomials)
              p.second = ps_.lweight(w, p.second);
          }
        return res;
      }

      /// Right-multiplication of \a lhs by \a w.
      value_t rweight(const value_t& lhs, const weight_t& w) const
      {
        auto res = value_t{ws_.mul(lhs.constant, w)};
        if (!ws_.is_zero(w))
          for (auto& p: lhs.polynomials)
            for (const auto& m: p.second)
              ps_.add_here(res.polynomials[p.first],
                           rs_.rweight(label_of(m), w), weight_of(m));
        return res;
      }

      /// In place right multiplication by an expression.
      value_t& rmul_label_here(value_t& res, const expression_t& rhs) const
      {
        assert(ws_.is_zero(res.constant));
        for (auto& p: res.polynomials)
          p.second = ps_.rmul_label(p.second, rhs);
        return res;
      }

      /// Inplace left-division by \a w of \a res.
      value_t& ldivide_here(const weight_t& w, value_t& res) const
      {
        res.constant = ws_.ldivide(w, res.constant);
        for (auto& p: res.polynomials)
          for (auto&& m: p.second)
            weight_set(m, ws_.ldivide(w, weight_of(m)));
        return normalize(res);
      }

    private:
      template <typename Conjunction>
      void
      conjunctions_with_one_(value_t& res,
                             const value_t& l, const value_t& r,
                             Conjunction conjunction) const
      {
        const auto one = ls_.one();

        // Left constant.
        //
        // `<k> & \e . [P]` => `\e . [<k>1 & P]`
        // because
        // `$ . [<k>1] & \e . [P]` => `\e . [<k>1 & P]`
        if (!ws_.is_zero(l.constant))
          for (const auto& rhs: r.polynomials)
            if (ls_.is_one(rhs.first))
              ps_.add_here(res.polynomials[rhs.first],
                           ps_.conjunction(ps_.lweight(l.constant, ps_.one()),
                                           rhs.second));

        // Label one from the lhs.
        {
          auto i = l.polynomials.find(one);
          if (i != std::end(l.polynomials))
            for (const auto& rhs: r.polynomials)
              if (!ls_.is_one(rhs.first))
                ps_.add_here(res.polynomials[one],
                             conjunction(i->second,
                                         ps_.lmul_label(rs_.atom(rhs.first),
                                                        rhs.second)));
        }

        // Right constant.
        //
        // `\e . [P] & <k>` => `\e . [P & <k>1]`
        if (!ws_.is_zero(r.constant))
          for (const auto& lhs: l.polynomials)
            if (ls_.is_one(lhs.first))
              ps_.add_here(res.polynomials[lhs.first],
                           ps_.conjunction(lhs.second,
                                           ps_.lweight(r.constant, ps_.one())));

        // Label one from the rhs.
        {
          auto i = r.polynomials.find(one);
          if (i != std::end(r.polynomials))
            for (const auto& lhs: l.polynomials)
              if (!ls_.is_one(lhs.first))
                ps_.add_here(res.polynomials[one],
                             conjunction(ps_.lmul_label(rs_.atom(lhs.first),
                                                        lhs.second),
                                         i->second));
        }
      }

      /// The conjunction of \a l and \a r.
      ///
      /// When labelset is letterized.
      template <typename LabelSet = labelset_t, typename Conjunction>
      auto conjunction_(value_t l, value_t r,
                        Conjunction conjunction) const
        -> std::enable_if_t<detail::is_letterized_t<LabelSet>{},
                       value_t>
      {
        auto res = value_t{ws_.mul(l.constant, r.constant)};
        for (const auto& p: zip_maps(l.polynomials, r.polynomials))
          res.polynomials[p.first]
            = conjunction(std::get<0>(p.second), std::get<1>(p.second));

        conjunctions_with_one_(res, l, r, conjunction);
        normalize(res);
        return res;
      }

      /// The conjunction of \a l and \a r.
      ///
      /// When labelset is not letterized.
      template <typename LabelSet = labelset_t, typename Conjunction>
      auto conjunction_(const value_t& lhs, const value_t& rhs,
                        Conjunction conjunction) const
        -> std::enable_if_t<!detail::is_letterized_t<LabelSet>{},
                       value_t>
      {
        auto res = value_t{ws_.mul(lhs.constant, rhs.constant)};
        for (const auto& l: lhs.polynomials)
          for (const auto& r: rhs.polynomials)
            {
              // The longest common prefix.
              auto lcp = ls_.lgcd(l.first, r.first);
              if (!ls_.is_one(lcp))
                {
                  auto left  = rs_.atom(ls_.ldivide(lcp, l.first));
                  auto right = rs_.atom(ls_.ldivide(lcp, r.first));
                  ps_.add_here(res.polynomials[lcp],
                               conjunction(ps_.lmul_label(left,  l.second),
                                           ps_.lmul_label(right, r.second)));
                }
            }
        normalize(res);
        return res;
      }

      /// The shuffle product of \a l and \a r.
      template <typename Shuffle>
      value_t& shuffle_(value_t& res,
                        const value_t& lhs_xpn, const expression_t& lhs_xpr,
                        const value_t& rhs_xpn, const expression_t& rhs_xpr,
                        Shuffle shuffle) const
      {
        // (i) lhs_xpn:rhs_xpr.
        for (const auto& p: lhs_xpn.polynomials)
          for (const auto& m: p.second)
            ps_.add_here(res.polynomials[p.first],
                         shuffle(label_of(m), rhs_xpr), weight_of(m));
        // (ii) lhs_xpr:rhs_xpn
        for (const auto& p: rhs_xpn.polynomials)
          for (const auto& m: p.second)
            ps_.add_here(res.polynomials[p.first],
                         shuffle(lhs_xpr, label_of(m)), weight_of(m));
        // FIXME: normalize?
        return res;
      }

    public:
      /// The conjunction of \a l and \a r.
      value_t conjunction(const value_t& l, const value_t& r) const
      {
        return conjunction_(l, r,
                            [this](const polynomial_t& l,
                                   const polynomial_t& r)
                            {
                              return ps_.conjunction(l, r);
                            });
      }

      /// The shuffle product of \a de and \a df.
      ///
      /// d(E) : F + E : d(F)
      value_t shuffle(const value_t& de, const expression_t& e,
                      const value_t& df, const expression_t& f) const
      {
        auto res = value_t{ws_.mul(de.constant, df.constant)};
        return shuffle_(res,
                        de, e, df, f,
                        [this](const expression_t& l, const expression_t& r)
                        {
                          return rs_.shuffle(l, r);
                        });
      }

      /// The infiltration product of \a l and \a r.
      value_t infiltrate(const value_t& de, const expression_t& e,
                         const value_t& df, const expression_t& f) const
      {
        // Conjunction part: de&:df.
        auto res =
          conjunction_(de, df,
                       [this](const polynomial_t& l, const polynomial_t& r)
                       {
                         return ps_.infiltrate(l, r);
                       });

        // Shuffle part: de&:f + e&:df.
        shuffle_(res,
                 de, e, df, f,
                 [this](const expression_t& l, const expression_t& r)
                 {
                   return rs_.infiltrate(l, r);
                 });
        // FIXME: normalize?
        return res;
      }

      /*--------------.
      | complement.   |
      `--------------*/

      /// The complement of v.
      value_t complement(const value_t& v) const
      {
        // We need an expansion whose firsts are letters.
        // So require a letterized labelset.
        return complement_<detail::is_letterized_t<labelset_t>{}>(v);
      }

    private:
      /// Complement on an invalid labelset.
      template <bool IsLetterized>
      std::enable_if_t<!IsLetterized, value_t>
      complement_(const value_t&) const
      {
        raise(me(),
              ": complement: labelset must be letterized: ", ls_);
      }

      /// Complement on a letterized labelset.
      template <bool IsLetterized>
      std::enable_if_t<IsLetterized, value_t>
      complement_(const value_t& v) const
      {
        auto res = value_t{ws_.is_zero(v.constant) ? ws_.one() : ws_.zero()};

        require(!has(v.polynomials, ls_.one()),
                me(), ": complement: expansion must be normalized: ",
                to_string(*this, v));

        // Turn the polynomials into expressions, and complement them.
        // The if constexpr is made for oneset.
        if constexpr (detail::has_generators_mem_fn<labelset_t>{})
         for (auto l: ls_.generators())
           {
             auto i = v.polynomials.find(l);
             res.polynomials[l] =
               ps_.complement(i == end(v.polynomials)
                              ? ps_.zero() : i->second);
           }
        return res;
      }

      /*-----------.
      | ldivide.   |
      `-----------*/

    public:
      /// Transpose an expansion.  The firsts must be reduced to one.
      value_t transpose(const value_t& v) const
      {
        auto res = value_t{ws_.transpose(v.constant)};
        for (const auto& p: v.polynomials)
          {
            VCSN_REQUIRE(ls_.is_one(p.first),
                         *this, ": cannot transpose an expansion "
                         "with proper firsts: ", to_string(*this, v));
            res.polynomials[p.first] = ps_.transpose(p.second);
          }
        return res;
      }

      value_t ldivide(value_t lhs, value_t rhs) const
      {
        denormalize(lhs);
        denormalize(rhs);
        auto res = value_t{ws_.mul(lhs.constant, rhs.constant)};
        auto one = ls_.one();
        auto& res_one = res.polynomials[one];

        // ε⊙[X_a \ Y_a + ...] for common firsts, included ε.
        for (const auto& p: zip_maps(lhs.polynomials, rhs.polynomials))
          ps_.add_ldivide_here(res_one,
                               std::get<0>(p.second),
                               std::get<1>(p.second));

        // If ε ∈ f(X) then ε⊙[X_ε \ (b Y_b) + ...]
        if (has(lhs.polynomials, one))
          for (const auto& rhsp: rhs.polynomials)
            if (!ls_.is_one(rhsp.first))
              ps_.add_ldivide_here(res_one,
                                   lhs.polynomials[one],
                                   ps_.lmul_label(rs_.atom(rhsp.first),
                                                  rhsp.second));

        // If ε ∈ f(Y) then ε⊙[(a X_a) \ Y_ε + ...]
        if (has(rhs.polynomials, one))
          for (const auto& lhsp: lhs.polynomials)
            if (!ls_.is_one(lhsp.first))
              ps_.add_ldivide_here(res_one,
                                   ps_.lmul_label(rs_.atom(lhsp.first),
                                                  lhsp.second),
                                   rhs.polynomials[one]);

        // It was handy to use res_one, but if it's zero, then remove it.
        if (ps_.is_zero(res_one))
          res.polynomials.erase(one);
        normalize(res);
        return res;
      }

    public:
      /// Turn the polynomials into (normalized) monomials.
      value_t
      determinize(const value_t& v) const
      {
        auto res = value_t{v.constant};
        for (const auto& lp: v.polynomials)
          res.polynomials[lp.first] = {ps_.determinize(lp.second)};
        return res;
      }

      /*---------------.
      | tuple(v...).   |
      `---------------*/

      /// The type of the expansionset for tape Tape.
      template <unsigned Tape>
      using project_t
        = expansionset<typename expressionset_t::template project_t<Tape>>;

      /// The expansionset for tape Tape.
      template <unsigned Tape>
      auto project() const
        -> project_t<Tape>
      {
        return {rs_.template project<Tape>()};
      }

      /// Denormalize a pack of one-tape expansions.
      template <typename... Expansions>
      struct tuple_impl
      {
        /// Denormalize on this tape: from expansion to pure
        /// polynomial.
        template <size_t Tape>
        auto denormalize_tape(const typename project_t<Tape>::value_t& e) const
          -> typename project_t<Tape>::polys_t
        {
          auto es = eset_.template project<Tape>();
          auto res = e;
          es.denormalize(res);
          VCSN_REQUIRE(es.expressionset().weightset()->is_zero(res.constant),
                       es, ": to-expansion: cannot denormalize ",
                       to_string(es, res),
                       ", need support for label one (the empty label)");
          return res.polynomials;
        }

        /// Denormalize on all these tapes.
        template <size_t... Tape>
        auto denormalize(std::tuple<const Expansions&...>& es,
                         detail::index_sequence<Tape...>) const
          -> std::tuple<typename project_t<Tape>::polys_t...>
        {
          using res_t = std::tuple<typename project_t<Tape>::polys_t...>;
          return res_t{denormalize_tape<Tape>(std::get<Tape>(es))...};
        }

        /// Entry point: Denormalize all these expansions.
        auto denormalize(const Expansions&... es) const
        {
          auto t = std::tuple<const Expansions&...>{es...};
          constexpr auto indices
            = detail::make_index_sequence<sizeof...(Expansions)>{};
          return denormalize(t, indices);
        }

        auto
        tuple(Expansions&&... es) const
          -> value_t
        {
          auto res = eset_.zero();
          auto polys = denormalize(std::forward<Expansions>(es)...);
          detail::cross_tuple
            ([&res, this](const auto&... ps)
             {
               auto l = label_t{ps.first...};
               eset_.ps_.add_here(res.polynomials[l],
                                  eset_.ps_.tuple(ps.second...));
             },
             polys);
          eset_.normalize(res);
          return res;
        }

        const expansionset& eset_;
      };

      /// The tuplization of single-tape expansions into a multitape
      /// expansion.
      ///
      /// Another implementation is possible, based on the following
      /// two-tape example taking e0 and e1, two single-tape expansions:
      ///
      /// auto res = value_t{ws_.mul(e0.constant, e1.constant)};
      /// for (const auto& p0: e0.polynomials)
      ///     for (const auto& p1: e1.polynomials)
      ///       {
      ///         auto l = label_t{p0.first, p1.first};
      ///         ps_.add_here(res.polynomials[l],
      ///                      ps_.tuple(p0.second, p1.second));
      ///       }
      /// if (!ws_.is_zero(e0.constant))
      ///   {
      ///     auto rs0 = detail::project<0>(rs_);
      ///     using p0_t = typename polynomialset_t::template project_t<0>;
      ///     auto p0 = p0_t{{rs0.one(), e0.constant}};
      ///     for (const auto& p1: e1.polynomials)
      ///       {
      ///         auto l = label_t{*rs0.labelset().one(),
      ///                          p1.first};
      ///         ps_.add_here(res.polynomials[l],
      ///                      ps_.tuple(p0, p1.second));
      ///       }
      ///   }
      /// if (!ws_.is_zero(e1.constant))
      ///   {
      ///     auto rs1 = detail::project<1>(rs_);
      ///     using p1_t = typename polynomialset_t::template project_t<1>;
      ///     auto p1 = p1_t{{rs1.one(), e1.constant}};
      ///     for (const auto& p0: e0.polynomials)
      ///       {
      ///         auto l = label_t{p0.first,
      ///                          *rs1.labelset().one()};
      ///         ps_.add_here(res.polynomials[l],
      ///                      ps_.tuple(p0.second, p1));
      ///       }
      ///   }
      /// return res;
      ///
      /// The first part, the two nested for-loops that deal with the
      /// polynomial part of the expansions, is easy to scale to
      /// variadic tuples (that's a Cartesian product). The second
      /// part, the two if's and loop that deal with the constant
      /// terms, is more tricky.
      ///
      /// Rather than making the code more complex, since the constant
      /// part of the expansions is just the weight of the unit
      /// polynomial for the unit label, let's denormalize the
      /// expansions, let the general laws apply, and then normalize
      /// the result.
      template <typename... Expansions>
      auto
      tuple(Expansions&&... es) const
        -> value_t
      {
        auto t = tuple_impl<Expansions...>{*this};
        return t.tuple(std::forward<Expansions>(es)...);
      }

      /// Project a multitape expansion.
      template <size_t Tape>
      auto project(const value_t& v) const
      {
        auto xs = project<Tape>();
        const auto& ps = xs.polynomialset();
        using res_t = typename decltype(xs)::value_t;
        auto res = res_t{};
        res.constant = v.constant;
        for (const auto& p: v.polynomials)
          ps.add_here(res.polynomials[ls_.template project<Tape>(p.first)],
                      ps_.template project<Tape>(p.second));
        // We might generate denormalized expansions, e.g., when
        // projecting the expansion of `\e|a`, `\e` is a label.
        xs.normalize(res);
        return res;
      }

      void
      compose_with_one_(value_t& res,
                        const value_t& l, const value_t& r) const
      {
        if (old_way_)
          return compose_with_one_old_(res, l, r);
        else
          return compose_with_one_new_(res, l, r);
      }

      void
      compose_with_one_old_(value_t& res,
                            const value_t& l, const value_t& r) const
      {
        assert(ws_.is_zero(l.constant));
        assert(ws_.is_zero(r.constant));
        const auto& ls0 = ls_.template set<0>();
        const auto& ls1 = ls_.template set<1>();

        // Handle lhs labels with one on the second tape.
        for (const auto& lhs: l.polynomials)
          if (ls1.is_one(std::get<1>(lhs.first)))
            for (const auto& rhs: r.polynomials)
              if (!ls0.is_one(std::get<0>(rhs.first)))
                // a|\e . [P1] @ b|c . [P2] becomes a|\e . [P1 @ (b|c)P2]
                ps_.add_here(res.polynomials[lhs.first],
                             ps_.compose(lhs.second,
                                         ps_.lmul_label(rs_.atom(rhs.first),
                                                        rhs.second)));
        // Handle rhs labels with one on the first tape.
        for (const auto& rhs: r.polynomials)
          if (ls0.is_one(std::get<0>(rhs.first)))
            for (const auto& lhs: l.polynomials)
              if (!ls1.is_one(std::get<1>(lhs.first)))
                // a|b . [P1] @ \e|c . [P2] becomes \e|c . [(a|b)P1 @ P2]
                ps_.add_here(res.polynomials[rhs.first],
                             ps_.compose(ps_.lmul_label(rs_.atom(lhs.first),
                                                        lhs.second),
                                         rhs.second));
      }

      void
      compose_with_one_new_(value_t& res,
                            const value_t& l, const value_t& r) const
      {
        const auto& ls0 = ls_.template set<0>();
        const auto& ls1 = ls_.template set<1>();

        // Handle lhs labels with one on the second tape.
        for (const auto& lhs: l.polynomials)
          if (ls1.is_one(std::get<1>(lhs.first)))
            for (const auto& rhs: r.polynomials)
              if (!ls0.is_one(std::get<0>(rhs.first)))
                // a|\e . [P1] @ b|c . [P2] becomes a|c . [P1 @ (b|\e)P2]
                {
                  // a|c.
                  auto l0 = ls_.tuple(ls_.template project<0>(lhs.first),
                                      ls_.template project<1>(rhs.first));
                  // b|\e.
                  auto l1 = ls_.tuple(ls_.template project<0>(rhs.first),
                                      ls_.template project<1>(lhs.first));
                  ps_.add_here(res.polynomials[l0],
                               ps_.compose(lhs.second,
                                           ps_.lmul_label(rs_.atom(l1),
                                                          rhs.second)));
                }
        // Left constant.
        //
        // `$|$ . [<k>1] @ \e|c . [P2]` => `$|c . [(\e|$)<k>1 @ P2]`
        // `<k> @ \e|c . [P2]` => `\e|c . [<k>1 @ P2]`
        if (!ws_.is_zero(l.constant))
          for (const auto& rhs: r.polynomials)
            if (ls0.is_one(std::get<0>(rhs.first)))
              ps_.add_here(res.polynomials[rhs.first],
                           ps_.compose(ps_.lweight(l.constant, ps_.one()),
                                       rhs.second));

        // Handle rhs labels with one on the first tape.
        for (const auto& rhs: r.polynomials)
          if (ls0.is_one(std::get<0>(rhs.first)))
            for (const auto& lhs: l.polynomials)
              if (!ls1.is_one(std::get<1>(lhs.first)))
                // a|b . [P1] @ \e|c . [P2] becomes a|c . [(\e|b)P1 @ P2]
                {
                  // a|c.
                  auto l0 = ls_.tuple(ls_.template project<0>(lhs.first),
                                      ls_.template project<1>(rhs.first));
                  // \e|b.
                  auto l1 = ls_.tuple(ls_.template project<0>(rhs.first),
                                      ls_.template project<1>(lhs.first));
                  ps_.add_here(res.polynomials[l0],
                               ps_.compose(ps_.lmul_label(rs_.atom(l1),
                                                          lhs.second),
                                           rhs.second));
                }

        // Right constant.
        //
        // `a|\e . [P1] @ $|$ . [<k>1] ` => `a|$ . [P1 @ ($|\e)<k>1]`
        // `a|\e . [P1] @ <k>` => `a|\e . [P1 @ <k>1]`.
        if (!ws_.is_zero(r.constant))
          for (const auto& lhs: l.polynomials)
            if (ls0.is_one(std::get<1>(lhs.first)))
              ps_.add_here(res.polynomials[lhs.first],
                           ps_.compose(lhs.second,
                                       ps_.lweight(r.constant, ps_.one())));
      }


      /// The composition of \a l and \a r.
      template <typename Ctx = context_t>
      auto compose(value_t l, value_t r) const
        -> std::enable_if_t<are_composable<Ctx, Ctx>()
                            && number_of_tapes<Ctx>::value == 2,
                            value_t>
      {
        // Tape of the lhs on which we compose.
        constexpr auto out = labelset_t::size() - 1;
        // Tape of the rhs on which we compose.
        constexpr auto in = 0;
        if (denorm_)
          {
            denormalize(l);
            denormalize(r);
          }
        auto res = value_t{ws_.mul(l.constant, r.constant)};
        for (const auto& lm: l.polynomials)
          for (const auto& rm: r.polynomials)
            if (ls_.template set<out>().equal(std::get<out>(label_of(lm)),
                                              std::get<in>(label_of(rm))))
              {
                auto l = ls_.compose(ls_, label_of(lm),
                                     ls_, label_of(rm));
                ps_.add_here(res.polynomials[l],
                             ps_.compose(lm.second, rm.second));
              }
        compose_with_one_(res, l, r);
        // Beware that we might have introduced some constant terms
        // (e.g., \e|x @ x|\e), and some polynomials equal to 0 (\e|x @ y|\e).
        normalize(res);
        return res;
      }


    private:
      /// The expressionset used for the expressions.
      expressionset_t rs_;
      /// Shorthand to the labelset.
      const labelset_t& ls_ = *rs_.labelset();
      /// Shorthand to the weightset.
      const weightset_t& ws_ = *rs_.weightset();
      /// The polynomialset for the polynomials.
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// Whether to running the old composition code.
      bool old_way_ = !!getenv("VCSN_OLDWAY");
      /// Denormalize if requested explicitly, or if running the old way.
      bool denorm_ = old_way_ || !!getenv("VCSN_DENORM");
    };
  }

  template <typename Context>
  expansionset<expressionset<Context>>
  make_expansionset(const expressionset<Context>& es)
  {
    return {es};
  }

  namespace detail
  {
    /// The join of two expansionsets.
    template <typename Ctx1, typename Ctx2>
    struct join_impl<expansionset<expressionset<Ctx1>>,
                     expansionset<expressionset<Ctx2>>>
    {
      using type = expansionset<expressionset<join_t<Ctx1, Ctx2>>>;

      static type join(const expansionset<expressionset<Ctx1>>& lhs,
                       const expansionset<expressionset<Ctx2>>& rhs)
      {
        return type(vcsn::join(lhs.expressionset(), rhs.expressionset()));
      }
    };
  }
}
