#ifndef VCSN_CORE_RAT_RATEXPSET_HH
# define VCSN_CORE_RAT_RATEXPSET_HH

# include <string>
# include <list>

# include <vcsn/ctx/context.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/printer.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/weights/q.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{

  /// A typed ratexp set.
  /// \tparam Context  the LabelSet and WeightSet types.
  template <typename Context>
  class ratexpset
  {
  public:
    using self_type = ratexpset;
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;
    using kind_t = typename context_t::kind_t;
    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using label_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;
    using const_visitor = rat::const_visitor<label_t, weight_t>;
    /// Type of ratexps.
# define DEFINE(Type)                                           \
    using Type ## _t = rat::Type<label_t, weight_t>
    DEFINE(node);
    DEFINE(leaf);
    DEFINE(zero);
    DEFINE(one);
    DEFINE(atom);
    DEFINE(inner);
    DEFINE(sum);
    DEFINE(prod);
    DEFINE(intersection);
    DEFINE(shuffle);
    DEFINE(star);
    DEFINE(lweight);
    DEFINE(rweight);
    DEFINE(complement);
# undef DEFINE
    template <rat::exp::type_t Type>
    using nary_t = rat::nary<Type, label_t, weight_t>;
    using ratexp_t = std::shared_ptr<const node_t>;

    using type_t = typename node_t::type_t;
    using ratexps_t = typename node_t::ratexps_t;

    /// The value this is a set of: typeful shared pointers.
    using value_t = typename node_t::value_t;

  public:
    /// Static description key.
    static std::string sname();
    /// Dynamic description key.
    std::string vname(bool full = true) const;
    /// Build from the description in \a is.
    static ratexpset make(std::istream& is);

    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    ratexpset(const context_t& ctx);

    const context_t& context() const;
    const labelset_ptr& labelset() const;
    const weightset_ptr& weightset() const;

    auto atom(const label_t& v) const
      -> value_t;

    /// When used as WeightSet for automata.
    bool is_zero(value_t v) const ATTRIBUTE_PURE;
    bool is_one(value_t v) const ATTRIBUTE_PURE;

    static constexpr bool is_commutative_semiring()
    {
      return false;
    }

    static constexpr bool show_one()
    {
      return false;
    }

    static constexpr star_status_t star_status()
    {
      return star_status_t::STARRABLE;
    }

    value_t conv(std::istream& is) const;
    value_t conv(b, typename b::value_t v) const;
    value_t conv(const q& ws, typename q::value_t v) const;
    template <typename Ctx2>
    value_t conv(const ratexpset<Ctx2>& ws,
                 typename ratexpset<Ctx2>::value_t v) const;

    value_t conv(self_type, value_t v) const;
    std::ostream& print(std::ostream& o, const value_t v,
			const std::string& format = "text") const;
    std::string format(const value_t v) const;

  public:
    /// Whether \a l < \a r.
    static bool less_than(value_t l, value_t r);
    // Concrete type implementation.
    value_t zero() const;
    value_t one() const;
    value_t add(value_t l, value_t r) const;
    value_t mul(value_t l, value_t r) const;
    value_t concat(value_t l, value_t r) const;
    value_t intersection(value_t l, value_t r) const;
    value_t shuffle(value_t l, value_t r) const;
    value_t star(value_t e) const;
    value_t complement(value_t e) const;
    value_t rmul(value_t e, const weight_t& w) const;
    value_t lmul(const weight_t& w, value_t e) const;
    value_t transpose(value_t e) const;

  private:
    /// Push \a v in \a res, applying associativity if possible.
    /// \tparam Type  the kind of ratexps on which to apply associativity.
    ///               Must be sum, intersection, shuffle, or prod.
    template <rat::exp::type_t Type>
    void gather(ratexps_t& res, value_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \tparam Type  the kind of ratexps on which to apply associativity.
    ///               Must be SUM or PROD.
    template <rat::exp::type_t Type>
    ratexps_t gather(value_t l, value_t r) const;

    /// If Context is LAW.
    value_t concat_(value_t l, value_t r, std::true_type) const;
    /// If Context is not LAW.
    value_t concat_(value_t l, value_t r, std::false_type) const;

  private:
    context_t ctx_;

    rat::exp::type_t type_ignoring_lweight_(value_t e) const;
    weight_t possibly_implicit_lweight_(value_t e) const;
    value_t unwrap_possible_lweight_(value_t e) const;
  };

  /// The meet of two ratexpsets.
  template <typename Ctx1, typename Ctx2>
  inline
  auto
  meet(const ratexpset<Ctx1>& a, const ratexpset<Ctx2>& b)
    -> ratexpset<decltype(meet(a.context(), b.context()))>
  {
    return meet(a.context(), b.context());
  }

  /// The union of two ratexpsets.
  template <typename Ctx1, typename Ctx2>
  inline
  auto
  join(const ratexpset<Ctx1>& a, const ratexpset<Ctx2>& b)
    -> ratexpset<decltype(join(a.context(), b.context()))>
  {
    return join(a.context(), b.context());
  }

  template <typename Context>
  inline
  ratexpset<Context>
  join(const ratexpset<Context>& a, const b&)
  {
    return a;
  }

  template <typename Context>
  inline
  ratexpset<Context>
  join(const b& a, const ratexpset<Context>& b)
  {
    return join(b, a);
  }

  template <typename Context>
  inline
  auto
  join(const ratexpset<Context>& rs, const q& ws)
      -> ratexpset<ctx::context<typename Context::labelset_t,
                                decltype(join(*rs.weightset(), ws))>>
  {
    using ctx_t = ctx::context<typename Context::labelset_t,
                               decltype(join(*rs.weightset(), ws))>;
    return ctx_t{*rs.labelset(), join(*rs.weightset(), ws)};
  }

  template <typename Context>
  inline
  auto
  join(const q& ws, const ratexpset<Context>& rs)
    -> decltype(join(rs, ws))
  {
    return join(rs, ws);
  }

} // namespace vcsn

# include <vcsn/core/rat/ratexpset.hxx>

#endif // !VCSN_CORE_RAT_RATEXPSET_HH
