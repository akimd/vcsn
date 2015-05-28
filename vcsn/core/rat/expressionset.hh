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

    using word_t = self_t;
    using letter_t = self_t;

  public:
    /// Static description key.
    static symbol sname();
    /// Build from the description in \a is.
    static self_t make(std::istream& is);

    /// Constructor.
    /// \param ctx        the generator set for the labels, and the weight set.
    /// \param identities the identities to guarantee
    ///                    FIXME: make this optional again?
    expressionset_impl(const context_t& ctx,
                       identities_t identities);

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept unknown letters
    /// \returns   the previous status.
    bool open(bool o) const;

    /// Accessor to the context.
    const context_t& context() const;

    /// Accessor to the identities set.
    identities_t identities() const;
    bool is_series() const;

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
    value_t conv(const zmin& ws, typename zmin::value_t v) const;
    template <typename Ctx2>
    value_t conv(const expressionset<Ctx2>& ws,
                 typename expressionset<Ctx2>::value_t v) const;

    /// Whether \a l < \a r.
    static bool less(value_t l, value_t r);

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
    value_t concat(value_t l, value_t r) const;
    value_t conjunction(value_t l, value_t r) const;
    value_t shuffle(value_t l, value_t r) const;
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
                        const std::string& format = "text") const;

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        {
          o << "\\mathsf{";
          switch (identities())
            {
            case identities_t::trivial:
              o << "RatE";
              break;
            case identities_t::series:
              o << "Series";
              break;
            default:
              assert(false);
            };
          o << "}[";
          context().print_set(o, format);
          o << ']';
        }
      else if (format == "text")
        {
          switch (identities_)
            {
            case identities_t::trivial:
              o <<  "expressionset<";
              break;
            case identities_t::series:
              o << "seriesset<";
              break;
            default:
              assert(false);
            }
          context().print_set(o, format);
          o << '>';
        }
      else
        raise("invalid format: ", format);
      return o;
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

    value_t remove_from_sum_series_(values_t addends,
                                    typename values_t::iterator i) const;
    value_t insert_in_sum_series_(const sum_t& addends, value_t r) const;
    value_t merge_sum_series_(const sum_t& addends1, value_t aa2) const;

    /// The sum of two non-zero series.
    /// \pre  !is_zero(l)
    /// \pre  !is_zero(r)
    value_t add_nonzero_series_(value_t l, value_t r) const;

    /// If e is an lweight, then its child, otherwise e.
    value_t unwrap_possible_lweight_(value_t e) const;
    /// The type of e, or the type of its child if e is a lweight.
    type_t type_ignoring_lweight_(value_t e) const;
    /// The weight of e if it's an lweight, otherwise the weight one().
    weight_t possibly_implicit_lweight_(value_t e) const;

    /// The product of l and r, using expression-identities.
    value_t mul_(value_t l, value_t r, bool series) const;

    /// The product of l and r, using expression-identities.
    value_t mul_expressions_(value_t l, value_t r) const;

    /// The product of l and r, using series-identities.
    value_t mul_series_(value_t l, value_t r) const;

    /// The product of l and r, using expression-identities.
    /// \pre l and r are non-trivial.
    value_t nontrivial_mul_expressions_(value_t l, value_t r) const;

    /// The product of l and r, using series-identities.
    /// \pre l and r are non-trivial.
    value_t nontrivial_mul_series_(value_t l, value_t r) const;

    /// Push \a v in \a res, applying associativity if possible.
    /// \tparam Type  the kind of expressions on which to apply associativity.
    ///               Must be sum, conjunction, shuffle, or prod.
    template <type_t Type>
    void gather_(values_t& res, value_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \tparam Type  the kind of expressions on which to apply associativity.
    ///               Must be SUM or PROD.
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

  private:
    context_t ctx_;
    const identities_t identities_;
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

    // Z.
    template <typename Context>
    struct join_impl<z, expressionset<Context>>
    {
      using context_t = context<labelset_t_of<Context>,
                                join_t<z, weightset_t_of<Context>>>;
      using type = expressionset<context_t>;
      static type join(const z& ws, const expressionset<Context>& rs)
      {
        return {context_t{*rs.labelset(), vcsn::join(ws, *rs.weightset())},
                rs.identities()};
      }
    };

    // Q.
    template <typename Context>
    struct join_impl<q, expressionset<Context>>
    {
      using context_t = context<labelset_t_of<Context>,
                                join_t<q, weightset_t_of<Context>>>;
      using type = expressionset<context_t>;
      static type join(const q& ws, const expressionset<Context>& rs)
      {
        return {context_t{*rs.labelset(), vcsn::join(ws, *rs.weightset())},
                rs.identities()};
      }
    };

    // R.
    template <typename Context>
    struct join_impl<r, expressionset<Context>>
    {
      using context_t = context<labelset_t_of<Context>,
                                join_t<r, weightset_t_of<Context>>>;
      using type = expressionset<context_t>;
      static type join(const r& ws, const expressionset<Context>& rs)
      {
        return {context_t{*rs.labelset(), vcsn::join(ws, *rs.weightset())},
                rs.identities()};
      }
    };

    // Zmin.
    template <typename Context>
    struct join_impl<zmin, expressionset<Context>>
    {
      using context_t = context<labelset_t_of<Context>,
                                join_t<zmin, weightset_t_of<Context>>>;
      using type = expressionset<context_t>;
      static type join(const zmin& ws, const expressionset<Context>& rs)
      {
        return {context_t{*rs.labelset(), vcsn::join(ws, *rs.weightset())},
                rs.identities()};
      }
    };

  }

  /// Shorthand to expressionset constructor.
  template <typename Context>
  expressionset<Context>
  make_expressionset(const Context& ctx, rat::identities identities)
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
