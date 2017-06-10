#pragma once

#include <set>
#include <string>

#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/core/rat/printer.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/nmin.hh>
#include <vcsn/weightset/q.hh>
#include <vcsn/weightset/r.hh>
#include <vcsn/weightset/z.hh>
#include <vcsn/weightset/zmin.hh>

#include <vcsn/core/rat/project.hh>

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

  public:
    /// Type of expressions.
    //
    // See http://stackoverflow.com/questions/15537023 to know why we
    // add the vcsn::rat:: part: GCC wants it, Clang does not care,
    // both are right.
#define DEFINE(Type)                                           \
    using Type ## _t = vcsn::rat::Type<context_t>
    DEFINE(add);
    DEFINE(atom);
    DEFINE(complement);
    DEFINE(compose);
    DEFINE(conjunction);
    DEFINE(infiltrate);
    DEFINE(inner);
    DEFINE(ldivide);
    DEFINE(leaf);
    DEFINE(lweight);
    DEFINE(name);
    DEFINE(node);
    DEFINE(one);
    DEFINE(mul);
    DEFINE(rweight);
    DEFINE(shuffle);
    DEFINE(star);
    DEFINE(transposition);
    DEFINE(tuple);
    DEFINE(zero);
#undef DEFINE

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
    static bool is_special(const value_t& v)
    {
      return equal(special(), v);
    }

    /// When used as a LabelSet.
    bool is_letter(value_t) const
    {
      return false;
    }

    /// Whether \a v is the `\\z`.
    ///
    /// Used when used as WeightSet.
    bool is_zero(const value_t& v) const ATTRIBUTE_PURE;

    /// Whether \a v is the `\\e`.
    ///
    /// Used when used as WeightSet.
    static bool is_one(const value_t& v) ATTRIBUTE_PURE;

    /// Whether \a v is the `0{c}`.
    bool is_universal(const value_t& v) const ATTRIBUTE_PURE;

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
    static constexpr bool has_lightening_weights()
    {
      return weightset_t::has_lightening_weights();
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
    auto conv(const letterset<GenSet>& ls,
                 typename letterset<GenSet>::value_t v) const -> value_t;
    auto conv(b, typename b::value_t v) const -> value_t;
    auto conv(const z& ws, typename z::value_t v) const -> value_t;
    auto conv(const q& ws, typename q::value_t v) const -> value_t;
    auto conv(const r& ws, typename r::value_t v) const -> value_t;
    auto conv(const nmin& ws, typename nmin::value_t v) const -> value_t;
    auto conv(const zmin& ws, typename zmin::value_t v) const -> value_t;
    template <typename Ctx2>
    auto conv(const expressionset<Ctx2>& ws,
                 typename expressionset<Ctx2>::value_t v) const -> value_t;

    /// The size of v
    static auto size(const value_t& v) -> size_t;

    /// Three-way comparison.
    static auto compare(const value_t& l, const value_t& r) -> int;

    /// Whether \a l < \a r.
    static auto less(const value_t& l, const value_t& r) -> bool;

    /// Whether \a l < \a r, ignoring lweight.
    ///
    /// Typically used for linear identities, where <2>a and <3>a are
    /// "equal" and should be merged into <5>a.
    static bool less_linear(const value_t& l, const value_t& r);

    /// Whether \a l == \a r.
    static auto equal(const value_t& l, const value_t& r) -> bool;

    /// Hash \a v.
    static auto hash(const value_t& v) -> size_t;

    /// Build a label.
    static auto atom(const label_t& v) -> value_t;

    /// Build a named expression.
    auto name(const value_t& v, symbol name) const -> value_t;

    // Concrete type implementation.
    static auto zero() -> value_t;
    static auto one() -> value_t;
    auto add(const value_t& l, const value_t& r) const -> value_t;
    auto mul(const value_t& l, const value_t& r) const -> value_t;

    /// Similar to mul, but in the case of LAW, merge the labels.
    ///
    ///   Mul(word(a),word(b)) -> mul(word(a), word(b))
    ///
    /// but
    ///
    ///   Concat(word(a),word(b)) -> word(ab)
    ///
    /// In other cases, synonym for mul.
    auto concat(const value_t& l, const value_t& r) const -> value_t;

    /// Build a composition: `l @ r`.
    auto compose(const value_t& l, const value_t& r) const -> value_t;

    /// Build an conjunction product: `l & r`.
    auto conjunction(const value_t& l, const value_t& r) const -> value_t;

    /// Build an infiltration product: `l &: r`.
    auto infiltrate(const value_t& l, const value_t& r) const -> value_t;

    /// Build a shuffle product: `l : r`.
    auto shuffle(const value_t& l, const value_t& r) const -> value_t;

    /// Build a tuple: `e | f | ...`.
    template <typename... Value>
    auto tuple(Value&&... v) const -> value_t;

    /// Add a power operator: `e{n}`.
    auto power(const value_t& e, unsigned n) const -> value_t;

    /// Build a left division: `l {\} r`.
    auto ldivide(const value_t& l, const value_t& r) const -> value_t;

    /// Build a right division: `l {/} r`.
    auto rdivide(const value_t& l, const value_t& r) const -> value_t;

    /// Add a star operator: `e*`.
    auto star(const value_t& e) const -> value_t;

    /// Add a complement operator: `e{c}`.
    auto complement(const value_t& e) const -> value_t;
    /// Add a transposition operator.
    auto transposition(const value_t& e) const -> value_t;
    /// Right-multiplication by a weight.
    auto rweight(const value_t& e, const weight_t& w) const -> value_t;
    /// Left-multiplication by a weight.
    auto lweight(const weight_t& w, const value_t& e) const -> value_t;
    /// The transposed of this rational expression.
    auto transpose(const value_t& e) const -> value_t;

    /// Make a `word' out of an expression
    auto word(label_t l) const -> word_t
    {
      return l;
    }

    /// An expression matching one character amongst \a chars.
    template <typename... Args>
    auto letter_class(Args&&... chars) const -> value_t;

    /// The next expression in a stream.
    auto conv(std::istream& is, bool = true) const -> value_t;

    /// Convert from ourself: identity.
    auto conv(const self_t&, const value_t& v) const -> value_t;

    /// Read a range of expressions.
    template <typename Fun>
    void convs(std::istream&, Fun) const
    {
      raise(*this, ": ranges not implemented");
    }

    auto print(const value_t& v,
               std::ostream& o = std::cout, format fmt = {}) const
      -> std::ostream&;

    /// Format the description of this expressionset.
    auto print_set(std::ostream& o, format fmt = {}) const
      -> std::ostream&;

    /// The type of the expressionset for the Tape-th tape.
    template <unsigned Tape, typename Ctx = context_t>
    using project_t
      = expressionset<detail::project_context<Tape, Ctx>>;

    /// The expressionset for the Tape-th tape.
    template <unsigned Tape>
    auto project() const
      -> project_t<Tape>
    {
      return vcsn::detail::project<Tape>(self());
    }

    /// Project a multitape expression.
    template <size_t Tape>
    auto project(const value_t& v) const
      -> decltype(::vcsn::rat::project<Tape>(this->self(), v))
    {
      return ::vcsn::rat::project<Tape>(self(), v);
    }

    template <typename Sequence>
    struct as_tupleset_impl;

    template <size_t... I>
    struct as_tupleset_impl<detail::index_sequence<I...>>
    {
      /// If we are multitape, our type as a tupleset.
      using type = tupleset<project_t<I, context_t>...>;

      static type value(const self_t& self)
      {
        return {detail::project<I>(self)...};
      }
    };

    /// If we are multitape, our type as a tupleset.
    template <typename Ctx = context_t>
    using as_tupleset_t
      = typename as_tupleset_impl<typename labelset_t_of<Ctx>::indices_t::type>::type;

    /// If we are multitape, ourself as a tupleset.
    ///
    /// This is used for instance when building a multitape
    /// expression: first we need the "sub" expressionsets.
    template <typename Ctx = context_t>
    auto as_tupleset() const
      -> std::enable_if_t<Ctx::is_lat, as_tupleset_t<Ctx>>
    {
      return as_tupleset_impl<typename labelset_t_of<Ctx>::indices_t::type>::value(self());
    }

  private:
    /// From a list of values, build a sum, taking care of the empty
    /// and singleton cases.
    auto add_(values_t&& vs) const -> value_t;

    auto add_linear_(const add_t& addends, const value_t& r) const -> value_t;
    auto add_linear_(const add_t& s1, const add_t& s2) const -> value_t;

    /// The sum of two non-zero series.
    /// \pre  !is_zero(l)
    /// \pre  !is_zero(r)
    auto add_linear_(const value_t& l, const value_t& r) const -> value_t;

    /// If e is an lweight, then its child, otherwise e.
    /// static because used by less.
    static auto unwrap_possible_lweight_(const value_t& e) -> value_t;
    /// The type of e, or the type of its child if e is a lweight.
    /// static because used by less.
    static type_t type_ignoring_lweight_(const value_t& e);
    /// The weight of e if it's an lweight, otherwise the weight one().
    /// static because used by less.
    static weight_t possibly_implicit_lweight_(const value_t& e);

    /// Push \a v in \a res, applying associativity if possible.
    /// \tparam Type  the kind of expressions on which to apply associativity.
    ///               Must be one of the variadic types.
    template <type_t Type>
    void gather_(values_t& res, const value_t& v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \tparam Type  the kind of expressions on which to apply associativity.
    ///               Must be one of the variadic types.
    template <type_t Type>
    values_t gather_(const value_t& l, const value_t& r) const;

    /// If labelset is wordset.
    auto concat_(const value_t& l, const value_t& r, std::true_type) const -> value_t;
    /// If labelset is not wordset.
    auto concat_(const value_t& l, const value_t& r, std::false_type) const -> value_t;

    /// If labelset is oneset.
    template <typename LabelSet_, typename... Args>
    auto letter_class_(const Args&&... chars, std::true_type) const -> value_t;

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
      label_t as_label(const tuple_t& v) const
      {
        return as_label_(v, labelset_t::indices);
      }

      /// Are all the components on I... labels?
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
      label_t as_label_(const tuple_t& v, detail::index_sequence<I...>) const
      {
        return label_t{as_label_<I>(v)...};
      }

      /// The expression on tape I is actually a label: get it.
      template <size_t I>
      typename project_t<I>::label_t as_label_(const tuple_t& v) const
      {
        if (std::get<I>(v.sub())->type() == type_t::one)
          return detail::label_one(rs_.labelset()->template set<I>());
        else
          return std::dynamic_pointer_cast<const typename project_t<I>::atom_t>
                 (std::get<I>(v.sub()))->value();
      }
      const self_t& rs_;
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
      using type1 = expressionset<Ctx1>;
      using type2 = expressionset<Ctx2>;
      using type = expressionset<join_t<Ctx1, Ctx2>>;

      static type join(const type1& a, const type2& b)
      {
        return {vcsn::join(a.context(), b.context()),
                vcsn::join(a.identities(), b.identities())};
      }
    };

    /// Join of a letterset and an expressionset.
    // FIXME: what about the other labelsets?
    template <typename GenSet1, typename Ctx2>
    struct join_impl<letterset<GenSet1>, expressionset<Ctx2>>
    {
      using type1 = letterset<GenSet1>;
      using type2 = expressionset<Ctx2>;
      using context_t = context<join_t<letterset<GenSet1>, labelset_t_of<Ctx2>>,
                                weightset_t_of<Ctx2>>;
      using type = expressionset<context_t>;

      static type join(const type1& a, const type2& b)
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
      static type join(const b&, const type& rhs)
      {
        return rhs;
      }
    };

    template <typename W1, typename W2>
    struct join_impl_simple;

    template <typename WeightSet, typename Context>
    struct join_impl_simple<WeightSet, expressionset<Context>>
    {
      using type1 = WeightSet;
      using type2 = expressionset<Context>;
      using context_t = context<labelset_t_of<Context>,
                                join_t<WeightSet, weightset_t_of<Context>>>;
      using type = expressionset<context_t>;
      static type join(const type1& ws, const type2& rs)
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
  template <typename LabelSet, typename WeightSet>
  auto
  make_expressionset(const context<LabelSet, WeightSet>& ctx,
                     rat::identities ids = {})
    -> expressionset<context<LabelSet, WeightSet>>
  {
    return {ctx, ids};
  }

  /// Shorthand to expressionset constructor.
  template <typename Context>
  auto
  make_expressionset(const expressionset<Context>& rs,
                     rat::identities ids = {})
    -> expressionset<Context>
  {
    return {rs.context(), ids};
  }

  /// Number of tapes of an expressionset.
  template <typename Context>
  struct is_multitape<expressionset<Context>>
    : is_multitape<Context>
  {};

  /// The meet of two expressionsets.
  template <typename Ctx1, typename Ctx2>
  auto
  meet(const expressionset<Ctx1>& a, const expressionset<Ctx2>& b)
    -> expressionset<meet_t<Ctx1, Ctx2>>
  {
    return {meet(a.context(), b.context()),
            meet(a.identities(), b.identities())};
  }


  /*----------------.
  | random_label.   |
  `----------------*/

  /// Random label from expressionset: limited to a single label.
  template <typename Context,
            typename RandomGenerator = std::default_random_engine>
  typename expressionset<Context>::value_t
  random_label(const expressionset<Context>& rs,
               RandomGenerator& gen = RandomGenerator())
  {
    return rs.atom(random_label(*rs.labelset(), gen));
  }
} // namespace vcsn

#include <vcsn/core/rat/expressionset.hxx>

// This is ugly, yet I don't know how to address this circular
// dependency another way: expressionset.hxx uses is-valid-expression.hh,
// which, of course, also uses expressionset.hh.
//
// So let's have expressionset.hh first accept a forward declaration (via
// algos/fwd.hh), then provide here the needed definition.  Do not
// leave this inside the CPP guard.

#include <vcsn/algos/is-valid-expression.hh>
