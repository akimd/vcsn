#ifndef VCSN_CORE_RAT_RATEXPSET_HH
# define VCSN_CORE_RAT_RATEXPSET_HH

# include <set>
# include <string>

# include <vcsn/core/rat/printer.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/weightset/b.hh>
# include <vcsn/weightset/z.hh>
# include <vcsn/weightset/q.hh>
# include <vcsn/weightset/r.hh>

namespace vcsn
{
  namespace rat
  {
  /// A typed ratexp set.
  /// \tparam Context  the LabelSet and WeightSet types.
  template <typename Context>
  class ratexpset_impl
  {
  public:
    using self_type = ratexpset<Context>;
    using context_t = Context;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;
    using kind_t = labels_are_ratexps;
    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using label_t = label_t_of<context_t>;
    using weight_t = typename weightset_t::value_t;
    using const_visitor = rat::const_visitor<label_t, weight_t>;
    /// Type of ratexps.
    //
    // See http://stackoverflow.com/questions/15537023 to know why we
    // add the vcsn::rat:: part: GCC wants it, Clang does not care,
    // both are right.
# define DEFINE(Type)                                           \
    using Type ## _t = vcsn::rat::Type<label_t, weight_t>
    DEFINE(atom);
    DEFINE(complement);
    DEFINE(inner);
    DEFINE(conjunction);
    DEFINE(leaf);
    DEFINE(ldiv);
    DEFINE(lweight);
    DEFINE(node);
    DEFINE(one);
    DEFINE(prod);
    DEFINE(rweight);
    DEFINE(shuffle);
    DEFINE(star);
    DEFINE(sum);
    DEFINE(transposition);
    DEFINE(zero);
# undef DEFINE
    template <exp::type_t Type>
    using unary_t = unary<Type, label_t, weight_t>;
    template <exp::type_t Type>
    using variadic_t = variadic<Type, label_t, weight_t>;
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
    static self_type make(std::istream& is);

    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    ratexpset_impl(const context_t& ctx);

    const context_t& context() const;
    const labelset_ptr& labelset() const;
    const weightset_ptr& weightset() const;

    static auto atom(const label_t& v)
      -> value_t;

    /// When used as a LabelSet for automata.
    static value_t special()
    {
      return atom(labelset_t::special());
    }

    /// When used as a LabelSet for automata.
    /// When used as WeightSet for automata.
    static bool is_special(value_t v)
    {
      return equals(special(), v);
    }

    bool is_letter(value_t) const
    {
      return false;
    }

    bool is_zero(value_t v) const ATTRIBUTE_PURE;
    static bool is_one(value_t v) ATTRIBUTE_PURE;

    static constexpr bool is_commutative_semiring()
    {
      return false;
    }

    static constexpr bool show_one()
    {
      return false;
    }

    static constexpr bool has_one()
    {
      return true;
    }

    static constexpr bool is_ratexpset()
    {
      return true;
    }

    static constexpr star_status_t star_status()
    {
      return star_status_t::STARRABLE;
    }

    value_t conv(std::istream& is) const;
    template <typename GenSet>
    value_t conv(const letterset<GenSet>& ls,
                 typename letterset<GenSet>::value_t v) const;
    value_t conv(b, typename b::value_t v) const;
    value_t conv(const z& ws, typename z::value_t v) const;
    value_t conv(const q& ws, typename q::value_t v) const;
    value_t conv(const r& ws, typename r::value_t v) const;
    template <typename Ctx2>
    value_t conv(const ratexpset_impl<Ctx2>& ws,
                 typename ratexpset_impl<Ctx2>::value_t v) const;

    value_t conv(self_type, value_t v) const;

    std::set<value_t> convs(std::istream&) const
    {
      raise(vname(), ": ranges not implemented");
    }

    std::ostream& print(std::ostream& o, const value_t v,
			const std::string& format = "text") const;

    std::ostream&
    print_set(std::ostream& o, const std::string& format) const
    {
      if (format == "latex")
        {
          o << "\\mathsf{RatE}[";
          context().print_set(o, format);
          o << ']';
        }
      else if (format == "text")
        o << vname();
      else
        raise("invalid format: ", format);
      return o;
    }

  public:
    /// Whether \a l < \a r.
    static bool less_than(value_t l, value_t r);

    /// Whether \a l == \a r.
    static bool equals(value_t l, value_t r);

    /// Hash \a l.
    static size_t hash(const value_t& l);

    // Concrete type implementation.
    value_t zero() const;
    static value_t one();
    value_t add(value_t l, value_t r) const;
    value_t mul(value_t l, value_t r) const;
    value_t concat(value_t l, value_t r) const;
    value_t conjunction(value_t l, value_t r) const;
    value_t shuffle(value_t l, value_t r) const;
    value_t ldiv(value_t l, value_t r) const;
    value_t rdiv(value_t l, value_t r) const;
    value_t star(value_t e) const;
    /// Add a complement operator.
    value_t complement(value_t e) const;
    /// Add a transposition operator.
    value_t transposition(value_t e) const;
    /// Right-multiplication by a weight.
    value_t rmul(value_t e, const weight_t& w) const;
    /// Left-multiplication by a weight.
    value_t lmul(const weight_t& w, value_t e) const;
    /// The transposed of this rational expression.
    value_t transpose(value_t e) const;

    /// A ratexp matching one character amongst \a chars.
    template <typename... Args>
    value_t letter_class(Args&&... chars) const;

  private:
    /// Push \a v in \a res, applying associativity if possible.
    /// \tparam Type  the kind of ratexps on which to apply associativity.
    ///               Must be sum, conjunction, shuffle, or prod.
    template <exp::type_t Type>
    void gather(ratexps_t& res, value_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \tparam Type  the kind of ratexps on which to apply associativity.
    ///               Must be SUM or PROD.
    template <exp::type_t Type>
    ratexps_t gather(value_t l, value_t r) const;

    /// If Context is LAW.
    value_t concat_(value_t l, value_t r, std::true_type) const;
    /// If Context is not LAW.
    value_t concat_(value_t l, value_t r, std::false_type) const;

    /// If context is oneset.
    template <typename LabelSet_, typename... Args>
    value_t letter_class_(const Args&&... chars, std::true_type) const;

    /// If context is not oneset.
    template <typename LabelSet_>
    value_t
    letter_class_(std::set<std::pair<typename LabelSet_::letter_t,
                                   typename LabelSet_::letter_t>> chars,
                bool accept,
                std::false_type) const;

  private:
    context_t ctx_;

    exp::type_t type_ignoring_lweight_(value_t e) const;
    weight_t possibly_implicit_lweight_(value_t e) const;
    value_t unwrap_possible_lweight_(value_t e) const;
  };
  } // rat::

  /// The meet of two ratexpsets.
  template <typename Ctx1, typename Ctx2>
  inline
  auto
  meet(const ratexpset<Ctx1>& a, const ratexpset<Ctx2>& b)
    -> ratexpset<meet_t<Ctx1, Ctx2>>
  {
    return meet(a.context(), b.context());
  }

  /// The union of two ratexpsets.
  template <typename Ctx1, typename Ctx2>
  inline
  auto
  join(const ratexpset<Ctx1>& a, const ratexpset<Ctx2>& b)
    -> ratexpset<join_t<Ctx1, Ctx2>>
  {
    return join(a.context(), b.context());
  }

  // Used as a labelset.
  template <typename GenSet1,  typename Ctx2>
  inline
  auto
  join(const letterset<GenSet1>& a, const ratexpset<Ctx2>& b)
    -> ratexpset<context<join_t<letterset<GenSet1>, labelset_t_of<Ctx2>>,
                         weightset_t_of<Ctx2>>>
  {
    using ctx_t = context<join_t<letterset<GenSet1>, labelset_t_of<Ctx2>>,
                          weightset_t_of<Ctx2>>;
    return ctx_t{join(a, *b.labelset()), *b.weightset()};
  }

  template <typename Ctx1,  typename GenSet2>
  inline
  auto
  join(const ratexpset<Ctx1>& a, const letterset<GenSet2>& b)
    -> decltype(join(b, a))
  {
    return join(b, a);
  }

  // B.  FIXME: screams for refactoring!

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

  // Z.
  template <typename Context>
  inline
  auto
  join(const ratexpset<Context>& rs, const z& ws)
      -> ratexpset<context<labelset_t_of<Context>,
                           join_t<weightset_t_of<Context>, z>>>
  {
    using ctx_t = context<labelset_t_of<Context>,
                          join_t<weightset_t_of<Context>, z>>;
    return ctx_t{*rs.labelset(), join(*rs.weightset(), ws)};
  }

  template <typename Context>
  inline
  auto
  join(const z& ws, const ratexpset<Context>& rs)
    -> join_t<ratexpset<Context>, z>
  {
    return join(rs, ws);
  }

  // Q.
  template <typename Context>
  inline
  auto
  join(const ratexpset<Context>& rs, const q& ws)
      -> ratexpset<context<labelset_t_of<Context>,
                           join_t<weightset_t_of<Context>, q>>>
  {
    using ctx_t = context<labelset_t_of<Context>,
                          join_t<weightset_t_of<Context>, q>>;
    return ctx_t{*rs.labelset(), join(*rs.weightset(), ws)};
  }

  template <typename Context>
  inline
  auto
  join(const q& ws, const ratexpset<Context>& rs)
    -> join_t<ratexpset<Context>, q>
  {
    return join(rs, ws);
  }

  // R.
  template <typename Context>
  inline
  auto
  join(const ratexpset<Context>& rs, const r& ws)
      -> ratexpset<context<labelset_t_of<Context>,
                           join_t<weightset_t_of<Context>, r>>>
  {
    using ctx_t = context<labelset_t_of<Context>,
                          join_t<weightset_t_of<Context>, r>>;
    return ctx_t{*rs.labelset(), join(*rs.weightset(), ws)};
  }

  template <typename Context>
  inline
  auto
  join(const r& ws, const ratexpset<Context>& rs)
  // FIXME: loops if wrong order.
    -> join_t<ratexpset<Context>, r>
  {
    return join(rs, ws);
  }

} // namespace vcsn

# include <vcsn/core/rat/ratexpset.hxx>

#endif // !VCSN_CORE_RAT_RATEXPSET_HH
