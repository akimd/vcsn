#pragma once

#include <algorithm>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

#include <boost/optional.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/lexicographical_compare.hpp>

#include <vcsn/ctx/context.hh> // We need context to define join.
#include <vcsn/ctx/project-context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/algorithm.hh> // front
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/math.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/wet.hh>
#include <vcsn/misc/zip-maps.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/f2.hh>
#include <vcsn/weightset/z.hh>

namespace vcsn
{
  // http://llvm.org/bugs/show_bug.cgi?id=18571
#if defined __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunused-value"
#endif
  template <typename LabelSet>
  auto label_is_zero(const LabelSet& ls, const typename LabelSet::value_t* l)
    -> decltype(ls.is_zero(l), bool())
  {
    return ls.is_zero(*l);
  }

#if defined __clang__
# pragma clang diagnostic pop
#endif

  template <typename LabelSet>
  bool label_is_zero(const LabelSet&, ...)
  ATTRIBUTE_CONST;

  template <typename LabelSet>
  bool label_is_zero(const LabelSet&, ...)
  {
    return false;
  }

  namespace detail
  {
    template <typename WeightSet>
    struct is_division_ring
      : std::true_type
    {};

    template <>
    struct is_division_ring<z>
      : std::false_type
    {};

    template <typename Context, wet_kind_t Kind>
    struct is_division_ring<polynomialset<Context, Kind>>
      : std::false_type
    {};

  /// Linear combination of labels: map labels to weights.
  /// \tparam Context  the LabelSet and WeightSet types.
  template <typename Context, wet_kind_t Kind>
  class polynomialset_impl
  {
  public:
    using self_t = polynomialset<Context, Kind>;
    using context_t = Context;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;

    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    /// Polynomials over labels.
    using label_t = typename labelset_t::value_t;
    using weight_t = weight_t_of<context_t>;

    using value_t = wet_of<context_t, Kind>;
    /// A pair <label, weight>.
    using monomial_t = typename value_t::value_type;

    polynomialset_impl() = delete;
    polynomialset_impl(const polynomialset_impl&) = default;
    polynomialset_impl(polynomialset_impl&&) = default;
    polynomialset_impl(const context_t& ctx)
      : ctx_{ctx}
    {}

    /// Ourself, but after the application of weightset_mixin.
    ///
    /// FIXME: this is ugly.  It is due to the fact that instead of the
    /// CRTP, we used a mixin approach to add features to expressionset
    /// as opposed to expressionset_impl.  Except that here, we have an
    /// expression_impl, and we need the expression.  So after all,
    /// maybe the CRTP is a better approach.
    ///
    /// Cannot be a reference member, as we do support assignments,
    /// in which case the copied self would point to the original this.
    const self_t& self() const { return static_cast<const self_t&>(*this); }

    /// The static name.
    static symbol sname()
    {
      static auto res = symbol{"polynomialset<" + context_t::sname() + '>'};
      return res;
    }

    const context_t& context() const { return ctx_; }
    const labelset_ptr& labelset() const { return ctx_.labelset(); }
    const weightset_ptr& weightset() const { return ctx_.weightset(); }

    static constexpr bool is_commutative() { return false; }
    static constexpr bool has_lightening_weights()
    {
      return weightset_t::has_lightening_weights();
    }

    /// Remove the monomial of \a l in \a v.
    value_t&
    del_weight(value_t& v, const label_t& l) const
    {
      v.erase(l);
      return v;
    }

    /// Set the monomial of \a l in \a v to weight \a k.
    /// \pre  w is not null
    value_t&
    new_weight(value_t& v, const label_t& l, const weight_t w) const
    {
      assert(!weightset()->is_zero(w));
      v.set(l, w);
      return v;
    }

    /// Set the monomial of \a l in \a v to weight \a w.
    value_t&
    set_weight(value_t& v, const label_t& l, const weight_t w) const
    {
      if (weightset()->is_zero(w))
        return del_weight(v, l);
      else
        return new_weight(v, l, w);
    }

    const weight_t
    get_weight(const value_t& v, const label_t& l) const ATTRIBUTE_PURE
    {
      auto i = v.find(l);
      if (i == v.end())
        return weightset()->zero();
      else
        return weight_of(*i);
    }


    /*---------.
    | clear.   |
    `---------*/

    /// Set to zero.
    void clear(value_t& v)
    {
      v.clear();
    }


    /*-------.
    | add.   |
    `-------*/

    /// `v += <k>l`.
    value_t&
    add_here(value_t& v, const label_t& l, const weight_t k) const
    {
      if (!label_is_zero(*labelset(), &l))
        {
          auto i = v.find(l);
          if (i == v.end())
            {
              set_weight(v, l, k);
            }
          else
            {
              // Do not use set_weight() because it would lookup l
              // again and we already have the right iterator.
              auto w2 = weightset()->add(weight_of(*i), k);
              if (weightset()->is_zero(w2))
                v.erase(i);
              else
                v.set(i, w2);
            }
        }
      return v;
    }

    /// `v += m`.
    value_t&
    add_here(value_t& v, const monomial_t& m) const
    {
      return add_here(v, label_of(m), weight_of(m));
    }

    /// `v += p`, default case.
    template <wet_kind_t WetType, typename WS>
    auto
    add_here_impl(value_t& l, const value_t& r) const
      -> std::enable_if_t<WetType != wet_kind_t::bitset,
                          value_t&>
    {
      for (const auto& m: r)
        add_here(l, m);
      return l;
    }

    /// `v += p`, B and bitsets.
    template <wet_kind_t WetType, typename WS>
    auto
    add_here_impl(value_t& l, const value_t& r) const
      -> std::enable_if_t<(WetType == wet_kind_t::bitset
                           && std::is_same<WS, b>::value),
                     value_t&>
    {
      l.set() |= r.set();
      return l;
    }

    /// `v += p`, F2 and bitsets.
    template <wet_kind_t WetType, typename WS>
    auto
    add_here_impl(value_t& l, const value_t& r) const
      -> std::enable_if_t<(WetType == wet_kind_t::bitset
                           && std::is_same<WS, f2>::value),
                          value_t&>
    {
      l.set() ^= r.set();
      return l;
    }

    value_t&
    add_here(value_t& l, const value_t& r) const
    {
      return add_here_impl<value_t::kind, weightset_t>(l, r);
    }

    /// The sum of polynomials \a l and \a r.
    value_t add(const value_t& l, const value_t& r) const
    {
      value_t res = l;
      add_here(res, r);
      return res;
    }


    /*-------.
    | sub.   |
    `-------*/

    /// `v -= m`.
    value_t&
    sub_here(value_t& v, const monomial_t& m) const
    {
      if (!label_is_zero(*labelset(), &label_of(m)))
        {
          auto i = v.find(label_of(m));
          if (i == v.end())
            {
              raise(*this, ": sub_here: invalid arguments: ",
                    to_string(*this, v), ", ", to_string(*this, m));
            }
          else
            {
              // Do not use set_weight() because it would lookup w
              // again and we already have the right iterator.
              auto w2 = weightset()->sub(weight_of(*i), weight_of(m));
              if (weightset()->is_zero(w2))
                v.erase(i);
              else
                weight_set(*i, w2);
            }
        }
      return v;
    }

    /// The subtraction of polynomials \a l and \a r.
    value_t
    sub(const value_t& l, const value_t& r) const
    {
      value_t res = l;
      for (const auto& rm: r)
        sub_here(res, rm);
      return res;
    }


    /*-------.
    | mul.   |
    `-------*/

    /// The product of monomials \a l and \a r.
    monomial_t
    mul(const monomial_t& l, const monomial_t& r) const
    {
      return {labelset()->mul(label_of(l), label_of(r)),
              weightset()->mul(weight_of(l), weight_of(r))};
    }

    /// The product of polynomials \a l and \a r.
    /// General case.
    template <wet_kind_t WetType>
    auto
    mul_impl(const value_t& l, const value_t& r) const
      -> std::enable_if_t<WetType != wet_kind_t::bitset,
                     value_t>
    {
      value_t res;
      for (const auto& lm: l)
        for (const auto& rm: r)
          add_here(res,
                   labelset()->mul(label_of(lm), label_of(rm)),
                   weightset()->mul(weight_of(lm), weight_of(rm)));
      return res;
    }

    /// The product of polynomials \a l and \a r.
    /// Case of bitsets.
    template <wet_kind_t WetType>
    auto
    mul_impl(const value_t& l, const value_t& r) const
      -> std::enable_if_t<WetType == wet_kind_t::bitset,
                     value_t>
    {
      return l.set() & r.set();
    }

    /// The product of polynomials \a l and \a r.
    auto
    mul(const value_t& l, const value_t& r) const
      -> value_t
    {
      return mul_impl<value_t::kind>(l, r);
    }

    /// The product of polynomials \a l and \a r.
    auto
    mul(const value_t& p, const label_t& l, const weight_t w) const
      -> value_t
    {
      value_t res;
      for (const auto& m: p)
        add_here(res,
                 labelset()->mul(label_of(m), l),
                 weightset()->mul(weight_of(m), w));
      return res;
    }


    /*---------------.
    | conjunction.   |
    `---------------*/

    /// The conjunction of polynomials \a l and \a r.
    /// Valid only for expressionsets.
    template <typename Ctx>
    std::enable_if_t<Ctx::is_lar, value_t>
    conjunction_impl(const value_t& l, const value_t& r) const
    {
      value_t res;
      for (const auto& lm: l)
        for (const auto& rm: r)
          add_here(res,
                   labelset()->conjunction(label_of(lm), label_of(rm)),
                   weightset()->mul(weight_of(lm), weight_of(rm)));
      return res;
    }

    /// The conjunction of polynomials \a l and \a r.
    /// Valid only for every other labelsets.
    template <typename Ctx>
    std::enable_if_t<!Ctx::is_lar, value_t>
    conjunction_impl(const value_t& l, const value_t& r) const
    {
      value_t res;
      for (const auto& p: zip_maps<vcsn::as_tuple>(l, r))
        add_here(res,
                 label_of(std::get<0>(p)),
                 weightset()->mul(weight_of(std::get<0>(p)),
                                  weight_of(std::get<1>(p))));
      return res;
    }

    value_t
    conjunction(const value_t& l, const value_t& r) const
    {
      return conjunction_impl<context_t>(l, r);
    }

    /// The infiltration of polynomials \a l and \a r.
    /// Not valid for all the labelsets.
    value_t
    infiltrate(const value_t& l, const value_t& r) const
    {
      value_t res;
      for (const auto& lm: l)
        for (const auto& rm: r)
          add_here(res,
                   labelset()->infiltrate(label_of(lm), label_of(rm)),
                   weightset()->mul(weight_of(lm), weight_of(rm)));
      return res;
    }

    /// The sum of the weights of the common labels.
    /// Default case.
    template <wet_kind_t WetType = value_t::kind>
    auto
    scalar_product(const value_t& l, const value_t& r) const
      -> std::enable_if_t<WetType != wet_kind_t::bitset,
                          weight_t>
    {
      auto res = weightset()->zero();
      for (const auto& p: zip_maps<vcsn::as_tuple>(l, r))
        res = weightset()->add(res,
                               weightset()->mul(weight_of(std::get<0>(p)),
                                                weight_of(std::get<1>(p))));
      return res;
    }

    /// The sum of the weights of the common labels.
    /// B and bitsets.
    template <wet_kind_t WetType = value_t::kind, typename WS = weightset_t>
    auto
    scalar_product(const value_t& l, const value_t& r) const
      -> std::enable_if_t<(WetType == wet_kind_t::bitset
                           && std::is_same<WS, b>::value),
                     weight_t>
    {
      return l.set().intersects(r.set());
    }

    /// The sum of the weights of the common labels.
    /// F2 and bitsets.
    template <wet_kind_t WetType = value_t::kind, typename WS = weightset_t>
    auto
    scalar_product(const value_t& l, const value_t& r) const
      -> std::enable_if_t<(WetType == wet_kind_t::bitset
                           && std::is_same<WS, f2>::value),
                          weight_t>
    {
      return (l.set() & r.set()).count() % 2;
    }

    /// The star of polynomial \a v.
    value_t
    star(const value_t& v) const
    {
      // The only starrable polynomials are scalars (if they are
      // starrable too).
      auto s = v.size();
      if (s == 0)
        return one();
      else if (s == 1)
        {
          auto i = v.find(labelset()->one());
          if (i != v.end())
            return {{i->first, weightset()->star(i->second)}};
        }
      raise_not_starrable(*this, v);
    }

    /// Left exterior product.
    value_t
    lweight(const weight_t w, const value_t& v) const
    {
      value_t res;
      if (weightset()->is_one(w))
        res = v;
      else if (!weightset()->is_zero(w))
        for (const auto& m: v)
          add_here(res, label_of(m), weightset()->mul(w, weight_of(m)));
      return res;
    }

    /// Left product by a label.
    value_t
    lmul_label(const label_t& lhs, const value_t& v) const
    {
      value_t res;
      for (const auto& m: v)
        add_here(res,
                 labelset()->mul(lhs, label_of(m)),
                 weight_of(m));
      return res;
    }

    /// Left product by a monomial.
    value_t
    mul(const monomial_t& lhs, const value_t& v) const
    {
      value_t res;
      for (const auto& m: v)
        add_here(res,
                 labelset()->mul(label_of(lhs), label_of(m)),
                 weightset()->mul(weight_of(lhs), weight_of(m)));
      return res;
    }

    /// Detect whether the labelset features `rweight`.
    template <typename Ctx>
    using rweight_t
    = decltype(std::declval<labelset_t_of<Ctx>>()
               .rweight(std::declval<label_t_of<Ctx>>(),
                        std::declval<weight_t_of<Ctx>>()));

    /// Whether LabelSet features `rweight`.
    template <typename Ctx>
    using has_rweight_fn = detect<Ctx, rweight_t>;

    /// Right exterior product.
    auto
    rweight(const value_t& v, const weight_t w) const
      -> value_t
    {
      value_t res;
      if (weightset()->is_one(w))
        res = v;
      else if (!weightset()->is_zero(w))
        for (const auto& m: v)
          // Beware that if the labelset supports weights (e.g.,
          // polynomial of expressions), we do not multiply the weight
          // here, but the label.
          static_if<has_rweight_fn<context_t>{}>
            ([this, &res] (const auto& ls, const auto& m, const auto& w)
             {
               add_here(res,
                        ls.rweight(label_of(m), w),
                        weight_of(m));
             },
             [this, &res] (const auto&, const auto& m, const auto& w)
             {
               add_here(res,
                        label_of(m),
                        weightset()->mul(w, weight_of(m)));
             })
            (*labelset(), m, w);
      return res;
    }

    /// Right product.
    value_t
    rmul_label(const value_t& v, const label_t& rhs) const
    {
      value_t res;
      for (const auto& lhs: v)
        add_here(res,
                 labelset()->mul(label_of(lhs), rhs),
                 weight_of(lhs));
      return res;
    }

    /// Right product by a monomial.
    value_t
    mul(const value_t& l, const monomial_t& rhs) const
    {
      value_t res;
      for (const auto& lhs: l)
        add_here(res,
                 labelset()->mul(label_of(lhs), label_of(rhs)),
                 weightset()->mul(weight_of(lhs), weight_of(rhs)));
      return res;
    }

    value_t
    rdiv(const value_t& l, const value_t& r) const
    {
      raise(*this, ": rdiv: not implemented (",
            to_string(*this, l), ", ", to_string(*this, r), ")");
    }

    monomial_t
    ldiv(const monomial_t& l, const monomial_t& r) const
    {
      return {labelset()->ldiv(label_of(l), label_of(r)),
              weightset()->ldiv(weight_of(l), weight_of(r))};
    }

    /// Left division by a monomial.
    value_t
    ldiv(const monomial_t& l, const value_t& r) const
    {
      value_t res;
      for (const auto& m: r)
        add_here(res, ldiv(l, m));
      return res;
    }

    value_t
    ldiv(const value_t& l, const value_t& r) const
    {
      value_t res;
      if (is_zero(l))
        raise(*this, ": ldiv: division by zero");
      else
        {
          value_t remainder = r;
#if DEBUG
          std::cerr << "ldiv(";
          print(l, std::cerr) << ", ";
          print(r, std::cerr) << "\n";
#endif
          while (!is_zero(remainder))
            {
              auto factor = ldiv(detail::front(l), detail::front(remainder));
#if DEBUG
              std::cerr << "factor = "; print(factor, std::cerr) << "\n";
#endif
              add_here(res, factor);
#if DEBUG
              std::cerr << "res = "; print(res, std::cerr) << "\n";
              std::cerr << "sub = "; print(mul(l, factor), std::cerr) << "\n";
#endif
              remainder = sub(remainder, mul(l, factor));
#if DEBUG
              std::cerr << "rem = "; print(remainder, std::cerr) << "\n";
#endif
            }
#if DEBUG
          std::cerr << "ldiv(";
          print(l, std::cerr) << ", ";
          print(r, std::cerr) << ") = ";
          print(res, std::cerr) << " rem: ";
          print(remainder, std::cerr) << "\n";
#endif
          if (!is_zero(remainder))
            raise(*this, ": ldiv: not implemented (",
                  to_string(*this, l), ", ", to_string(*this, r), ")");
        }
      return res;
    }

    /// Left exterior division.
    value_t&
    ldiv_here(const weight_t w, value_t& v) const
    {
      if (!weightset()->is_one(w))
        for (auto&& m: v)
          weight_set(m, weightset()->ldiv(w, weight_of(m)));
      return v;
    }

    /// Right exterior division.
    value_t&
    rdiv_here(value_t& v, const weight_t w) const
    {
      if (!weightset()->is_one(w))
        for (auto& m: v)
          weight_set(m, weightset()->rdiv(weight_of(m), w));
      return v;
    }

    /// LGCD between two polynomials.
    ///
    /// Based only on weights.
    /// For instance <2>a+<4>b, <3>a+<6>b => <1>a+<2>b.
    /// And ab, a => 1.
    /// We could try to have ab, a => a in the future.
    value_t lgcd(const value_t& lhs, const value_t& rhs) const
    {
      using std::begin;
      using std::end;
      value_t res;
      // For each monomial, look for the matching GCD of the weight.
      auto i = begin(lhs), i_end = end(lhs);
      auto j = begin(rhs), j_end = end(rhs);
      for (;
           i != i_end && j != j_end
             && labelset()->equal(i->first, j->first);
           ++i, ++j)
        res.set(i->first, weightset()->lgcd(i->second, j->second));
      // If the sets of labels are different, the polynomials
      // cannot be "colinear", and the GCD is just 1.
      if (i != i_end || j != j_end)
        res = one();
      return res;
    }

    /*--------.
    | norm.   |
    `--------*/

    /// In the general case, normalize by the first (non null) weight.
    template <typename WeightSet, typename Dummy = void>
    struct norm_
    {
      typename WeightSet::value_t operator()(const value_t& v) const
      {
        return weight_of(front(v));
      }
      const WeightSet& ws_;
    };

    /// For Z, take the GCD, with the sign of the first value.
    template <typename Dummy>
    struct norm_<z, Dummy>
    {
      typename z::value_t operator()(const value_t& v) const
      {
        int sign = 0 < weight_of(detail::front(v)) ? 1 : -1;
        auto res = abs(weight_of(detail::front(v)));
        for (const auto& m: v)
          res = z_.lgcd(res, abs(weight_of(m)));
        res *= sign;
        return res;
      }
      const z& z_;
    };

    /// Compute the left GCD of weights which are polynomials.
    template <typename Ctx, wet_kind_t Knd, typename Dummy>
    struct norm_<polynomialset<Ctx, Knd>, Dummy>
    {
      using ps_t = polynomialset<Ctx, Knd>;

      typename ps_t::value_t operator()(const value_t& v) const
      {
        typename ps_t::value_t res = weight_of(detail::front(v));
        for (const auto& p: v)
          res = ps_.lgcd(res, weight_of(p));
        return res;
      }
      const ps_t& ps_;
    };

    /// The norm: the weight with which we should divide a polynomial
    /// to normalize it.
    auto norm(const value_t& v) const
      -> weight_t
    {
      if (is_zero(v))
        return weightset()->zero();
      else
        return norm_<weightset_t>{*weightset()}(v);
    }


    /*-------------.
    | normalize.   |
    `-------------*/

    /// Normalize v in place: compute the LGCD of the weights, ldiv
    /// the monomials with that factor, and return the factor.
    weight_t normalize_here(value_t& v) const
    {
      // Zero is in normal form, don't try to divide by zero.
      auto res = norm(v);
      if (!weightset()->is_zero(res))
        ldiv_here(res, v);
      return res;
    }

    /// Normalized v.
    value_t normalize(value_t res) const
    {
      normalize_here(res);
      return res;
    }



    /*---------------.
    | tuple(v...).   |
    `---------------*/

    /// Build a tuple of polynomials: (e.E+f.F)|(g.G+h.H)
    /// => eg.(E|G) + eh.(E|H) + fg.(F|G) + fh.(F|H).
    template <typename... Polys>
    auto
    tuple(Polys&&... vs) const
      -> value_t
    {
      auto res = value_t{};
      detail::cross([&res, this](auto... ms)
                    {
                      this->add_here(res,
                                     this->labelset()->tuple(ms.first...),
                                     this->weightset()->mul(ms.second...));
                    },
                    std::forward<Polys>(vs)...);
      return res;
    }

    /// The polynomialset for tape Tape.
    template <size_t Tape>
    auto project() const
    {
      return make_polynomialset(vcsn::detail::project<Tape>(context()));
    }

    /// Extract a single tape.
    template <size_t Tape>
    auto project(const value_t& v) const
    {
      auto ps = project<Tape>();
      auto res = ps.zero();
      for (const auto& m: v)
        ps.add_here(res,
                    labelset()->template project<Tape>(label_of(m)),
                    weight_of(m));
      return res;
    }

    /*-----------.
    | compose.   |
    `-----------*/

    /// Detect whether the labelset features `compose`.
    template <typename Ctx>
    using compose_t
    = decltype(std::declval<labelset_t_of<Ctx>>()
               .compose(std::declval<label_t_of<Ctx>>(),
                        std::declval<label_t_of<Ctx>>()));

    /// Whether LabelSet features `compose`.
    template <typename Ctx>
    using has_compose_fn = detect<Ctx, compose_t>;


    /// The composition of polynomials \a l and \a r when the context is a
    /// composable tupleset.
    template <typename Ctx = context_t>
    auto
    compose(const value_t& l, const value_t& r) const
      -> std::enable_if_t<are_composable<Ctx, Ctx>{}, value_t>
    {
      value_t res;
      for (const auto& lm: l)
        for (const auto& rm: r)
          if (labelset()->template set<0>().equal(std::get<1>(label_of(lm)),
                                                  std::get<0>(label_of(rm))))
            add_here(res, labelset()->tuple(std::get<0>(label_of(lm)),
                                            std::get<1>(label_of(rm))),
                     weightset()->mul(weight_of(lm), weight_of(rm)));
      return res;
    }

    /// The composition of polynomials \a l and \a r when the context features
    /// `compose`.
    template <typename Ctx = context_t>
    auto
    compose(const value_t& l, const value_t& r) const
      -> std::enable_if_t<has_compose_fn<Ctx>{}, value_t>
    {
      value_t res;
      for (const auto& lm: l)
        for (const auto& rm: r)
          add_here(res, labelset()->compose(label_of(lm), label_of(rm)),
                                            weightset()->mul(weight_of(lm),
                                                             weight_of(rm)));
      return res;
    }



    /// Convert into a label.
    ///
    /// Requires a rather powerful labelset, typically expressionset.
    /// Typical usage is therefore to "project" a polynomial of
    /// expressions into an expression (say for complement for
    /// instance).
    label_t to_label(const value_t& v) const
    {
      label_t res = labelset()->zero();
      for (const auto& m: v)
        res = labelset()->add(res,
                              labelset()->lweight(weight_of(m), label_of(m)));
      return res;
    }

    /// "Determinize" this polynomial: turn into a monomial.
    ///
    /// Requires a rather powerful labelset, typically expressionset.
    monomial_t determinize(value_t v) const
    {
      weight_t w = normalize_here(v);
      return {to_label(v), w};
    }

    /// Complement this polynomial.
    ///
    /// Requires a rather powerful labelset, typically expressionset.
    value_t complement(const value_t& v) const
    {
      return {{labelset()->complement(to_label(normalize(v))),
               weightset()->one()}};
    }

    /*---------------.
    | equal(l, r).   |
    `---------------*/

    ATTRIBUTE_PURE
    static bool monomial_equal(const monomial_t& lhs,
                               const monomial_t& rhs)
    {
      return (labelset_t::equal(label_of(lhs), label_of(rhs))
              && weightset_t::equal(weight_of(lhs), weight_of(rhs)));
    }

    template <wet_kind_t WetType>
    ATTRIBUTE_PURE
    static auto
    equal_impl(const value_t& l, const value_t& r)
      -> std::enable_if_t<WetType != wet_kind_t::bitset,
                     bool>
    {
      return boost::equal(l, r, monomial_equal);
    }

    template <wet_kind_t WetType>
    ATTRIBUTE_PURE
    static auto
    equal_impl(const value_t& l, const value_t& r)
      -> std::enable_if_t<WetType == wet_kind_t::bitset,
                     bool>
    {
      return l.set() == r.set();
    }

    ATTRIBUTE_PURE
    static bool
    equal(const value_t& l, const value_t& r)
    {
      return equal_impl<value_t::kind>(l, r);
    }

    /// The unit polynomial.
    static const value_t& one()
    {
      static value_t res{monomial_one()};
      return res;
    }

    /// The unit monomial.
    static const monomial_t& monomial_one()
    {
      static monomial_t res{labelset_t::one(), weightset_t::one()};
      return res;
    }

    /// Whether is the unit polynomial.
    static bool is_one(const value_t& v) ATTRIBUTE_PURE
    {
      if (v.size() != 1)
        return false;
      auto i = v.find(labelset_t::one());
      if (i == v.end())
        return false;
      return weightset_t::is_one(i->second);
    }

    const value_t&
    zero() const
    {
      static value_t res;
      return res;
    }

    bool
    is_zero(const value_t& v) const
    {
      return v.empty();
    }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status()
    {
      return weightset_t::star_status();
    }

    /// Conversion from (this and) other weightsets.
    static value_t
    conv(self_t, const value_t& v)
    {
      return v;
    }

    /// FIXME: use enable_if to prevent this from being instantiated
    /// when WS is a polynomialset.  Then use this same technique for
    /// expressions.
    template <typename WS>
    value_t
    conv(const WS& ws, const typename WS::value_t& v) const
    {
      return {{labelset()->one(), weightset()->conv(ws, v)}};
    }

    /// Convert from another polynomialset to type_t.
    template <typename C, wet_kind_t K>
    value_t
    conv(const polynomialset<C, K>& sps,
         const typename polynomialset<C, K>::value_t& v) const
    {
      const typename C::labelset_t&  sls = *sps.labelset();
      const typename C::weightset_t& sws = *sps.weightset();
      const labelset_t&  tls = *labelset();
      const weightset_t& tws = *weightset();
      value_t res;
      for (const auto& m: v)
        add_here(res, tls.conv(sls, label_of(m)), tws.conv(sws, weight_of(m)));
      return res;
    }


    /*--------------.
    | less(l, r).   |
    `--------------*/

    ATTRIBUTE_PURE
    static bool monomial_less(const monomial_t& lhs, const monomial_t& rhs)
    {
      if (labelset_t::less(label_of(lhs), label_of(rhs)))
        return true;
      else if (labelset_t::less(label_of(rhs), label_of(lhs)))
        return false;
      else
        return weightset_t::less(weight_of(lhs), weight_of(rhs));
    }

    template <wet_kind_t WetType>
    ATTRIBUTE_PURE
    static auto
    less_impl(const value_t& l, const value_t& r)
      -> std::enable_if_t<WetType != wet_kind_t::bitset,
                     bool>
    {
      return boost::range::lexicographical_compare(l, r, monomial_less);
    }

    template <wet_kind_t WetType>
    ATTRIBUTE_PURE
    static auto
    less_impl(const value_t& l, const value_t& r)
      -> std::enable_if_t<WetType == wet_kind_t::bitset,
                     bool>
    {
      return l.set() < r.set();
    }

    ATTRIBUTE_PURE
    static bool
    less(const value_t& l, const value_t& r)
    {
      return less_impl<value_t::kind>(l, r);
    }


    value_t
    transpose(const value_t& v) const
    {
      value_t res;
      for (const auto& i: v)
        res.set(labelset()->transpose(label_of(i)),
                weightset()->transpose(weight_of(i)));
      return res;
    }


    /*--------.
    | hash.   |
    `--------*/
    ATTRIBUTE_PURE
    static size_t hash(const monomial_t& m, size_t res = 0)
    {
      hash_combine(res, labelset_t::hash(label_of(m)));
      hash_combine(res, weightset_t::hash(weight_of(m)));
      return res;
    }

    template <wet_kind_t WetType>
    ATTRIBUTE_PURE
    static auto
    hash_impl(const value_t& p)
      -> std::enable_if_t<WetType != wet_kind_t::bitset,
                     size_t>
    {
      size_t res = 0;
      for (const auto& m: p)
        res = hash(m, res);
      return res;
    }

    template <wet_kind_t WetType>
    ATTRIBUTE_PURE
    static auto
    hash_impl(const value_t& p)
      -> std::enable_if_t<WetType == wet_kind_t::bitset,
                     size_t>
    {
      return hash_value(p.set());
    }

    ATTRIBUTE_PURE
    static size_t hash(const value_t& v)
    {
      return hash_impl<value_t::kind>(v);
    }


    /// Build from the description in \a is.
    static self_t make(std::istream& is)
    {
      // name is, for instance, "polynomialset<lal_char(abcd), z>".
      eat(is, "polynomialset<");
      auto ctx = Context::make(is);
      eat(is, '>');
      return {ctx};
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathsf{Poly}[";
          context().print_set(o, fmt);
          o << ']';
          break;
        case format::sname:
          o << "polynomialset<";
          context().print_set(o, fmt);
          o << '>';
          break;
        case format::text:
        case format::utf8:
          o << "Poly[";
          context().print_set(o, fmt);
          o << ']';
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }

    /// Read a label, if there is one.
    ///
    /// Does not handle `\z`, nor letter classes.
    ///
    /// \returns  none if there is no label.
    boost::optional<label_t>
    conv_label(std::istream& i, bool weighted, const char sep = '+') const
    {
      int peek = i.peek();
      assert(peek != '[');
      if (peek == '\\')
        {
          i.ignore();
          if (i.peek() == 'z')
            {
              i.ignore();
              return boost::none;
            }
          else
            i.unget();
        }

      // The label is not \z.
      // Check if there is a label that comes.  Or rather, check if
      // there is something else than EOF or the separator, in which
      // case it must be a label.
      label_t res;
      if (peek == EOF || peek == sep || isspace(peek))
        {
          // There is no label.  This counts as '$', the special
          // label.
          //
          // Indeed, that's how we represent the initial and final
          // transitions: '$ -> 0 "<2>"'.  Using the one label is
          // tempting, but it does not exist for lal_char for
          // instance.  And it would be wrong to have '\e' when we
          // can, and '$' otherwise...
          //
          // However, we must have at least a weight: a completely
          // empty mononial ($ -> 0 "<2>,") is invalid.
          VCSN_REQUIRE(weighted,
                       *this, ": conv: invalid monomial: ",
                       str_escape(peek),
                       " (did you mean \\e or \\z?)");
          res = labelset()->special();
        }
      else
        {
          auto pos = i.tellg();
          res = labelset()->conv(i);
          // In law_char, when reading the monomial `a|b` (yes, `|` is
          // not escaped), we looped for ever: the `a` was read by
          // setalpha::get_word, which then returned, and then
          // conv_label repeatedly called get_word on `|b`, which
          // endlessly returned the empty word, refusing to pass the
          // `|`.
          //
          // Make sure we catch this.  Beware that tellg returns -1
          // (yes, signed!) on EOF.
          require(i.peek() == EOF || pos < i.tellg(),
                  *this, ": invalid implicit empty word before: ", i);
        }
      return res;
    }

    /// Read a weight, if there is one, bracketed.
    weight_t
    conv_weight(std::istream& i) const
    {
      if (i.peek() == langle)
        // FIXME: convert to use conv(std::istream).
        //
        // The problem is when we have a rational expression as a
        // weight: in that case, conv expect to parse up to EOF, not
        // up to '>'.  We first need to fix the parsing of expression
        // to work on a flow, to be able to use weightset()->conv
        // here.  Which means to get back the stream from a Flex
        // scanner.  It might not be easy.
        return ::vcsn::conv(*weightset(), bracketed(i, langle, rangle));
      else
        return weightset()->one();
    }

    /// Read a monomial from a stream.
    ///
    /// \param i    the stream to parse
    /// \param sep  the separator between monomials.
    ///
    /// \returns boost::none on EOF
    boost::optional<monomial_t>
    conv_monomial(std::istream& i, const char sep = '+') const
    {
#define SKIP_SPACES()                           \
      while (isspace(i.peek()))                 \
        i.ignore()

      // Nothing to read: signal EOF as an empty result.
      SKIP_SPACES();
      if (i.peek() == EOF)
        return boost::none;

      // Possibly a weight in braces.
      bool weighted = i.peek() == langle;
      weight_t w = conv_weight(i);

      // Possibly, a label.
      SKIP_SPACES();
      auto l = conv_label(i, weighted, sep);
      require(l, *this, ": \\z is invalid for monomials");
      return monomial_t{*l, w};
#undef SKIP_SPACES
    }

    /// Read a polynomial from a stream.
    ///
    /// Somewhat more general than a mere reversal of "format",
    /// in particular "a+a" is properly understood as "<2>a" in
    /// char_z.
    ///
    /// \param i    the stream to parse.
    /// \param sep  the separator between monomials.
    value_t
    conv(std::istream& i, const char sep = '+') const
    {
      value_t res;
#define SKIP_SPACES()                           \
      while (isspace(i.peek()))                 \
        i.ignore()

      do
        {
          // Possibly a weight in braces.
          SKIP_SPACES();
          bool weighted = i.peek() == langle;
          weight_t w = conv_weight(i);

          SKIP_SPACES();
          // Possibly, a label.
          // Handle label classes.
          if (i.peek() == '[')
            labelset()->convs(i, [this, &res, &w](const label_t& l)
                              {
                                add_here(res, l, w);
                              });
          else if (auto l = conv_label(i, weighted, sep))
            {
              require(l, *this, ": \\z is invalid for monomials");
              add_here(res, *l, w);
            }

          // sep (e.g., '+'), or stop parsing.
          SKIP_SPACES();
          if (i.peek() == sep)
            i.ignore();
          else
            break;
        }
      while (true);
#undef SKIP_SPACES

      return res;
    }

    /// Print a monomial.
    std::ostream&
    print(const monomial_t& m, std::ostream& out,
          format fmt = {}) const
    {
      static bool parens = getenv("VCSN_PARENS");
      print_weight_(weight_of(m), out, fmt);
      if (parens)
        out << (fmt == format::latex ? "\\left(" : "(");
      labelset()->print(label_of(m), out, fmt.for_labels());
      if (parens)
        out << (fmt == format::latex ? "\\right)" : ")");
      return out;
    }

    /// Print a value (a polynomial).
    ///
    /// \param v       the polynomial
    /// \param out     the output stream
    /// \param fmt     the format: "text" or "latex"
    /// \param sep     the separator between monomials
    std::ostream&
    print(const value_t& v, std::ostream& out,
          format fmt = {},
          const std::string& sep = " + ") const
    {
      if (is_zero(v))
        out << (fmt == format::latex ? "\\emptyset"
                : fmt == format::utf8 ? "∅"
                : "\\z");
      else
        {
          const auto s =
            (sep == " + "
             ? (fmt == format::latex ? std::string{" \\oplus "}
                : fmt == format::utf8 ? std::string{"⊕"}
                : sep)
             : sep);
          print_(v, out, fmt, s);
        }
      return out;
    }

  private:
    /// Print a weight.
    std::ostream&
    print_weight_(const weight_t w, std::ostream& out,
                  format fmt) const
    {
      static bool parens = getenv("VCSN_PARENS");
      if (parens || weightset()->show_one() || !weightset()->is_one(w))
        {
          out << (fmt == format::latex ? "\\left\\langle "
                  : fmt == format::utf8 ? "⟨"
                  : "<");
          weightset()->print(w, out, fmt.for_weights());
          out << (fmt == format::latex ? "\\right\\rangle "
                  : fmt == format::utf8 ? "⟩"
                  : ">");
        }
      return out;
    }

    /// Print a polynomial value without classes.
    std::ostream&
    print_without_classes_(const value_t& v, std::ostream& out,
                           format fmt,
                           const std::string& sep) const
    {
      bool first = true;
      for (const auto& m: v)
        {
          if (!first)
            out << sep;
          first = false;
          print(m, out, fmt);
        }
      return out;
    }

    /// Print a polynomial value with classes.
    std::ostream&
    print_with_classes_(const value_t& v, std::ostream& out,
                        format fmt,
                        const std::string& sep) const
    {
      using std::begin;
      using std::end;

      // We can use a vector, as we know that the labels are already
      // sorted, and random access iteration will be handy below.
      using labels_t = std::vector<label_t>;

      // Cluster the letters per weight.
      auto per_weight = std::map<weight_t, labels_t,
                                 vcsn::less<weightset_t>>{};
      // No classes if the weights of the letters aren't all the same.
      for (const auto& m: v)
        if (!labelset()->is_one(label_of(m)))
          per_weight[weight_of(m)].emplace_back(label_of(m));

      // Sort the clusters per label.
      auto per_label = std::map<label_t,
                                std::pair<weight_t, labels_t>,
                                vcsn::less<labelset_t>>{};
      for (const auto& p: per_weight)
        // Split classes which are too small.
        if (p.second.size() < 3)
          for (auto l: p.second)
            per_label[l] = std::make_pair(p.first, labels_t{l});
        else
          per_label[detail::front(p.second)] = p;

      // Whether we must not issue a separator.
      bool first = true;

      // Print with classes.  First, the constant-term.
      if (labelset()->is_one(label_of(detail::front(v))))
        {
          print(detail::front(v), out, fmt);
          first = false;
        }

      for (const auto& p: per_label)
        {
          if (!first)
            out << sep;
          first = false;

          // The weight.
          print_weight_(p.second.first, out, fmt);

          if (1 < p.second.second.size())
            // Print the character class.  'letters' are sorted, since
            // polynomials are shortlex-sorted on the labels.
            print_label_class(*labelset(), p.second.second,
                              out, fmt.for_labels());
          else
            labelset()->print(detail::front(p.second.second),
                              out, fmt.for_labels());
        }
      return out;
    }

    /// Print a non-null value for a non letterized labelset.
    template <typename Ctx = context_t>
    std::enable_if_t<!labelset_t_of<Ctx>::is_letterized(),
                      std::ostream&>
    print_(const value_t& v, std::ostream& out,
           format fmt = {},
           const std::string& sep = " + ") const
    {
      return print_without_classes_(v, out, fmt, sep);
    }

    /// Print a non-null value for a letterized labelset (e.g., letterset
    /// or nullableset.
    template <typename Ctx = context_t>
    std::enable_if_t<labelset_t_of<Ctx>::is_letterized(),
                      std::ostream&>
    print_(const value_t& v, std::ostream& out,
           format fmt = {},
           const std::string& sep = " + ") const
    {
      // No classes if not at least 3 elements.
      if (sep == " + " || v.size() <= 2)
        return print_without_classes_(v, out, fmt, sep);
      else
        return print_with_classes_(v, out, fmt, sep);
    }


  private:
    context_t ctx_;

    /// Left marker for weight in concrete syntax.
    constexpr static char langle = '<';
    /// Right marker for weight in concrete syntax.
    constexpr static char rangle = '>';
  };

    template <typename Context,
              wet_kind_t Kind = detail::wet_kind<labelset_t_of<Context>,
                                                 weightset_t_of<Context>>()>
    polynomialset<Context, Kind>
    make_polynomialset(const Context& context)
    {
      return {context};
    }

    template <typename Ctx1, wet_kind_t Kind1,
              typename Ctx2, wet_kind_t Kind2>
    struct join_impl<polynomialset<Ctx1, Kind1>,
                     polynomialset<Ctx2, Kind2>>
    {
      // Use the default kind.
      using type = polynomialset<join_t<Ctx1, Ctx2>>;
      static type join(const polynomialset<Ctx1, Kind1>& ps1,
                       const polynomialset<Ctx2, Kind2>& ps2)
      {
        return {vcsn::join(ps1.context(), ps2.context())};
      }
    };

    template <typename Ctx1, wet_kind_t Kind1,
              typename WS2>
    struct join_impl<polynomialset<Ctx1, Kind1>, WS2>
    {
      using type
        = polynomialset<context<typename Ctx1::labelset_t,
                                join_t<WS2, typename Ctx1::weightset_t>>>;
      static type join(const polynomialset<Ctx1, Kind1>& ps1, const WS2& ws2)
      {
        return {*ps1.labelset(), vcsn::join(*ps1.weightset(), ws2)};
      }
    };
  }
}
