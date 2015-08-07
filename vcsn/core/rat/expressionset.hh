#ifndef VCSN_CORE_RAT_EXPRESSIONSET_HH
# define VCSN_CORE_RAT_EXPRESSIONSET_HH

# include <set>
# include <string>

# include <vcsn/core/rat/identities.hh>
# include <vcsn/core/rat/printer.hh>
# include <vcsn/core/rat/expression.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/labelset/labelset.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star-status.hh>
# include <vcsn/weightset/b.hh>
# include <vcsn/weightset/nmin.hh>
# include <vcsn/weightset/q.hh>
# include <vcsn/weightset/r.hh>
# include <vcsn/weightset/z.hh>
# include <vcsn/weightset/zmin.hh>

namespace vcsn
{
  namespace rat
  {
  /// A typed expression set.
  /// \tparam Context  the LabelSet and WeightSet types.
  template <typename Context>
  class expressionset_impl
  {
  public:
    using self_t = expressionset<Context>;
    using context_t = Context;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;
    using kind_t = labels_are_expressions;
    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using label_t = label_t_of<context_t>;
    using weight_t = typename weightset_t::value_t;
    using identities_t = rat::identities;
    using const_visitor = rat::const_visitor<context_t>;

    /// Type of expressions.
    //
    // See http://stackoverflow.com/questions/15537023 to know why we
    // add the vcsn::rat:: part: GCC wants it, Clang does not care,
    // both are right.
# define DEFINE(Type)                                           \
    using Type ## _t = vcsn::rat::Type<context_t>
    DEFINE(atom);
    DEFINE(complement);
    DEFINE(conjunction);
    DEFINE(inner);
    DEFINE(ldiv);
    DEFINE(leaf);
    DEFINE(lweight);
    DEFINE(node);
    DEFINE(one);
    DEFINE(prod);
    DEFINE(rweight);
    DEFINE(shuffle);
    DEFINE(star);
    DEFINE(sum);
    DEFINE(tuple);
    DEFINE(transposition);
    DEFINE(zero);
# undef DEFINE

    /// An expression (a shared pointer to a tree).
    using value_t = typename node_t::value_t;
    /// Type tag for AST classes.
    using type_t = typename node_t::type_t;
    /// A list (vector) of expressions.
    using values_t = typename node_t::values_t;

    template <type_t Type>
    using unary_t = unary<Type, context_t>;
    template <type_t Type>
    using variadic_t = variadic<Type, context_t>;

    using word_t = value_t;
    using letter_t = value_t;

  public:
    /// Static description key.
    static symbol sname();
    /// Build from the description in \a is.
    static self_t make(std::istream& is);

    /// Constructor.
    /// \param ctx  the generator set for the labels, and the weight set.
    /// \param ids  the identities to guarantee
    expressionset_impl(const context_t& ctx, identities_t ids = {});

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept unknown letters
    /// \returns   the previous status.
    bool open(bool o) const;

    /// Accessor to the context.
    const context_t& context() const;

    /// Accessor to the identities set.
    identities_t identities() const;

    /// Accessor to the labelset.
    const labelset_ptr& labelset() const;
    /// Accessor to the weightset.
    const weightset_ptr& weightset() const;

    /// When used as a LabelSet.
    static value_t special()
    {
      return atom(labelset_t::special());
    }

    /// When used as a LabelSet.
    static bool is_special(value_t v)
    {
      return equal(special(), v);
    }

    /// When used as a LabelSet.
    bool is_letter(value_t) const
    {
      return false;
    }

    /// When used as WeightSet.
    bool is_zero(value_t v) const ATTRIBUTE_PURE;

    /// When used as WeightSet.
    static bool is_one(value_t v) ATTRIBUTE_PURE;

    /// When used as a labelset.
    static constexpr bool is_letterized()
    {
      return false;
    }

    /// When used as WeightSet.
    static constexpr bool is_commutative()
    {
      return false;
    }

    /// When used as WeightSet.
    static constexpr bool is_idempotent()
    {
      // FIXME: well, the truth is that we are idempotent if the
      // weightset is, _and_ we apply ACI to addition.
      return weightset_t::is_idempotent();
    }

    /// When used as WeightSet.
    static constexpr bool show_one()
    {
      return false;
    }

    /// When used as WeightSet.
    static constexpr bool has_one()
    {
      return true;
    }

    /// When used as WeightSet.
    static constexpr bool is_expressionset()
    {
      return true;
    }

    /// When used as WeightSet.
    static constexpr bool is_free()
    {
      return false;
    }

    /// When used as WeightSet.
    static constexpr star_status_t star_status()
    {
      return star_status_t::STARRABLE;
    }

    template <typename GenSet>
    value_t conv(const letterset<GenSet>& ls,
                 typename letterset<GenSet>::value_t v) const;
    value_t conv(b, typename b::value_t v) const;
    value_t conv(const z& ws, typename z::value_t v) const;
    value_t conv(const q& ws, typename q::value_t v) const;
    value_t conv(const r& ws, typename r::value_t v) const;
    value_t conv(const nmin& ws, typename nmin::value_t v) const;
    value_t conv(const zmin& ws, typename zmin::value_t v) const;
    template <typename Ctx2>
    value_t conv(const expressionset<Ctx2>& ws,
                 typename expressionset<Ctx2>::value_t v) const;

    /// Whether \a l < \a r.
    static bool less(value_t l, value_t r);

    /// Whether \a l < \a r, ignoring lweight.
    ///
    /// Typically used for linear identities, where <2>a and <3>a are
    /// "equal" and should be merged into <5>a.
    static bool less_linear(value_t l, value_t r);

    /// Whether \a l == \a r.
    static bool equal(value_t l, value_t r);

    /// Hash \a l.
    static size_t hash(const value_t& l);

    /// Build a label.
    static auto atom(const label_t& v)
      -> value_t;

    // Concrete type implementation.
    value_t zero() const;
    static value_t one();
    value_t add(value_t l, value_t r) const;
    value_t mul(value_t l, value_t r) const;

    /// Similar to mul, but in the case of LAW, merge the labels.
    ///
    ///   Mul(word(a),word(b)) -> prod(word(a), word(b))
    ///
    /// but
    ///
    ///   Concat(word(a),word(b)) -> word(ab)
    ///
    /// In other case, synonym for mul.
    value_t concat(value_t l, value_t r) const;

    value_t conjunction(value_t l, value_t r) const;
    value_t shuffle(value_t l, value_t r) const;
    template <typename... Value>
    value_t tuple(Value&&... v) const;
    /// Add a power operator.
    value_t power(value_t e, unsigned n) const;
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

    /// Make a `word' out of an expression
    word_t word(label_t l) const
    {
      return l;
    }

    /// An expression matching one character amongst \a chars.
    template <typename... Args>
    value_t letter_class(Args&&... chars) const;

    /// The next expression in a stream.
    value_t conv(std::istream& is) const;

    /// Convert from ourself: identity.
    value_t conv(const self_t&, value_t v) const;

    /// Read a range of expressions.
    template <typename Fun>
    static void convs(std::istream&, Fun)
    {
      raise(sname(), ": ranges not implemented");
    }

    std::ostream& print(const value_t v, std::ostream& o,
                        format fmt = {}) const;

    /// Format the description of this expressionset.
    std::ostream& print_set(std::ostream& o,
                            format fmt = {}) const;

    /// The type of the expressionset for the Tape-th tape.
    template <unsigned Tape, typename Ctx = context_t>
    using focus_t
      = expressionset<detail::focus_context<Tape, Ctx>>;


    template <typename Sequence>
    struct as_tupleset_impl;

    template <size_t... I>
    struct as_tupleset_impl<detail::index_sequence<I...>>
    {
      /// If we are multitape, our type as a tupleset.
      using type = tupleset<focus_t<I, context_t>...>;

      static type value(const self_t& self)
      {
        return {detail::make_focus<I>(self)...};
      }
    };

    /// If we are multitape, our type as a tupleset.
    template <typename Ctx = context_t>
    using as_tupleset_t
      = typename as_tupleset_impl<typename labelset_t_of<Ctx>::indices_t::type>::type;

    /// If we are multitape, ourself as a tupleset.
    template <typename Ctx = context_t>
    auto as_tupleset() const
      -> enable_if_t<Ctx::is_lat, as_tupleset_t<Ctx>>
    {
      return as_tupleset_impl<typename labelset_t_of<Ctx>::indices_t::type>::value(self());
    }

  private:
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

    /// From a list of values, build a sum, taking care of the empty
    /// and singleton cases.
    value_t add_(values_t&& vs) const;

    value_t add_linear_(const sum_t& addends, value_t r) const;
    value_t add_linear_(const sum_t& s1, const sum_t& s2) const;

    /// The sum of two non-zero series.
    /// \pre  !is_zero(l)
    /// \pre  !is_zero(r)
    value_t add_linear_(value_t l, value_t r) const;

    /// If e is an lweight, then its child, otherwise e.
    static value_t unwrap_possible_lweight_(value_t e);
    /// The type of e, or the type of its child if e is a lweight.
    static type_t type_ignoring_lweight_(value_t e);
    /// The weight of e if it's an lweight, otherwise the weight one().
    static weight_t possibly_implicit_lweight_(value_t e);

    /// Push \a v in \a res, applying associativity if possible.
    /// \tparam Type  the kind of expressions on which to apply associativity.
    ///               Must be one of the variadic types.
    template <type_t Type>
    void gather_(values_t& res, value_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \tparam Type  the kind of expressions on which to apply associativity.
    ///               Must be one of the variadic types.
    template <type_t Type>
    values_t gather_(value_t l, value_t r) const;

    /// If labelset is wordset.
    value_t concat_(value_t l, value_t r, std::true_type) const;
    /// If labelset is not wordset.
    value_t concat_(value_t l, value_t r, std::false_type) const;

    /// If labelset is oneset.
    template <typename LabelSet_, typename... Args>
    value_t letter_class_(const Args&&... chars, std::true_type) const;

    /// If labelset is not oneset.
    template <typename LabelSet_>
    value_t
    letter_class_(std::set<std::pair<typename LabelSet_::letter_t,
                                     typename LabelSet_::letter_t>> chars,
                  bool accept,
                  std::false_type) const;

    /// Turn a tuple of expressions that are labels into a multi-tape label.
    template <typename Dummy = void>
    struct tuple_of_label
    {
      /// Are all the components labels?
      static bool is_label(const tuple_t& v)
      {
        return is_label_(v, labelset_t::indices);
      }

      /// All the components are (single-tape) labels: make this a
      /// multitape label.
      static label_t as_label(const tuple_t& v)
      {
        return as_label_(v, labelset_t::indices);
      }

      template <size_t... I>
      static bool is_label_(const tuple_t& v, detail::index_sequence<I...>)
      {
        for (auto b: {(std::get<I>(v.sub())->type() == type_t::atom
                       || (std::get<I>(v.sub())->type() == type_t::one
                           && labelset_t::template valueset_t<I>::has_one()))...})
          if (!b)
            return false;
        return true;
      }

      template <size_t... I>
      static label_t as_label_(const tuple_t& v, detail::index_sequence<I...>)
      {
        return label_t{as_label_<I>(v)...};
      }

      template <size_t I>
      static typename focus_t<I>::label_t as_label_(const tuple_t& v)
      {
        if (std::get<I>(v.sub())->type() == type_t::one)
          return detail::label_one<typename labelset_t::template valueset_t<I>>();
        else
          return std::dynamic_pointer_cast<const typename focus_t<I>::atom_t>
                 (std::get<I>(v.sub()))->value();
      }
    };

  private:
    /// The context of the expressions.
    context_t ctx_;
    /// The set of rewriting rules to apply.
    const identities_t ids_;
  };
  } // rat::

  namespace detail
  {
    /// Conversion to a nullableset: identity.
    template <typename Ctx>
    struct nullableset_traits<expressionset<Ctx>>
    {
      using type = expressionset<Ctx>;
      static type value(const type& ls)
      {
        return ls;
      }
    };

    /// Conversion to a wordset: identity.
    template <typename Ctx>
    struct law_traits<expressionset<Ctx>>
    {
      using type = expressionset<Ctx>;
      static type value(const type& ls)
      {
        return ls;
      }
    };

    /// The join of two expressionsets.
    template <typename Ctx1, typename Ctx2>
    struct join_impl<expressionset<Ctx1>, expressionset<Ctx2>>
    {
      using type = expressionset<join_t<Ctx1, Ctx2>>;

      static type join(const expressionset<Ctx1>& lhs,
                       const expressionset<Ctx2>& rhs)
      {
        return {vcsn::join(lhs.context(), rhs.context()),
                vcsn::join(lhs.identities(), rhs.identities())};
      }
    };

    /// Join of a letterset and an expressionset.
    // FIXME: what about the other labelsets?
    template <typename GenSet1,  typename Ctx2>
    struct join_impl<letterset<GenSet1>, expressionset<Ctx2>>
    {
      using context_t = context<join_t<letterset<GenSet1>, labelset_t_of<Ctx2>>,
                                weightset_t_of<Ctx2>>;
      using type = expressionset<context_t>;

      static type join(const letterset<GenSet1>& a,
                       const expressionset<Ctx2>& b)
      {
        return {context_t{vcsn::join(a, *b.labelset()), *b.weightset()},
                          b.identities()};
      }
    };

    // B.  FIXME: screams for refactoring!
    template <typename Context>
    struct join_impl<b, expressionset<Context>>
    {
      using type = expressionset<Context>;
      static type join(const b&, const expressionset<Context>& rhs)
      {
        return rhs;
      }
    };

    template <typename W1, typename W2>
    struct join_impl_simple;

    template <typename WeightSet, typename Context>
    struct join_impl_simple<WeightSet, expressionset<Context>>
    {
      using context_t = context<labelset_t_of<Context>,
                                join_t<WeightSet, weightset_t_of<Context>>>;
      using type = expressionset<context_t>;
      static type join(const WeightSet& ws, const expressionset<Context>& rs)
      {
        return {context_t{*rs.labelset(), vcsn::join(ws, *rs.weightset())},
                rs.identities()};
      }
    };
#define JOIN_IMPL_SIMPLE(WS)                                    \
    template <typename Context>                                 \
    struct join_impl<WS, expressionset<Context>>                \
      : public join_impl_simple<WS, expressionset<Context>>     \
    {};


    JOIN_IMPL_SIMPLE(z);
    JOIN_IMPL_SIMPLE(q);
    JOIN_IMPL_SIMPLE(r);
    JOIN_IMPL_SIMPLE(zmin);

#undef JOIN_IMPL_SIMPLE

  }

  /// Shorthand to expressionset constructor.
  template <typename Context>
  expressionset<Context>
  make_expressionset(const Context& ctx, rat::identities identities = {})
  {
    return {ctx, identities};
  }

  /// The meet of two expressionsets.
  template <typename Ctx1, typename Ctx2>
  inline
  auto
  meet(const expressionset<Ctx1>& a, const expressionset<Ctx2>& b)
    -> expressionset<meet_t<Ctx1, Ctx2>>
  {
    return {meet(a.context(), b.context()),
            meet(a.identities(), b.identities())};
  }

} // namespace vcsn

# include <vcsn/core/rat/expressionset.hxx>

#endif // !VCSN_CORE_RAT_EXPRESSIONSET_HH

// This is ugly, yet I don't know how to address this circular
// dependency another way: expressionset.hxx uses is-valid-expression.hh,
// which, of course, also uses expressionset.hh.
//
// So let's have expressionset.hh first accept a forward declaration (via
// algos/fwd.hh), then provide here the needed definition.  Do not
// leave this inside the CPP guard.

#include <vcsn/algos/is-valid-expression.hh>
