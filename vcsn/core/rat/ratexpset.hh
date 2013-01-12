#ifndef VCSN_CORE_RAT_RATEXPSET_HH
# define VCSN_CORE_RAT_RATEXPSET_HH

# include <string>
# include <list>

# include <vcsn/ctx/ctx.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/printer.hh>

namespace vcsn
{

  /// An abstract factory for ratexp.
  template <typename Context>
  class ratexpset
  {
  public:
    using context_t = Context;
    using labelset_t = typename context_t::labelset_t;
    using weightset_t = typename context_t::weightset_t;
    using kind_t = typename context_t::kind_t;
    using labelset_ptr = typename context_t::labelset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using letter_t = typename labelset_t::letter_t;
    using word_t = typename labelset_t::word_t;
    using label_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;
    /// Type of printer visitor.
    using printer_t = rat::printer<context_t>;
    /// Type of ratexps.
# define DEFINE(Type)                                           \
    using Type ## _t = rat::Type<label_t, weight_t>
    DEFINE(node);
    DEFINE(leaf);
    DEFINE(zero);
    DEFINE(one);
    DEFINE(atom);
    DEFINE(inner);
    DEFINE(nary);
    DEFINE(sum);
    DEFINE(prod);
    DEFINE(star);
# undef DEFINE

    using type_t = typename node_t::type_t;
    using ratexps_t = typename node_t::ratexps_t;

    /// The value this is a set of: typeful shared pointers.
    using value_t = typename node_t::value_t;

  public:
    /// Static description key.
    static std::string sname();
    /// Dynamic description key.
    virtual std::string vname(bool full = true) const;

    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    ratexpset(const context_t& ctx);

    const context_t& context() const;
    const labelset_ptr& labelset() const;
    const weightset_ptr& weightset() const;

    auto atom(const label_t& v) const
      -> value_t;

    /// When used as WeightSet for automata.
    bool is_zero(value_t v) const;
    bool is_unit(value_t v) const;

    static constexpr bool show_unit()
    {
      return false;
    }

    static constexpr bool is_positive_semiring()
    {
      return weightset_t::is_positive_semiring();
    }

    value_t conv(const std::string& s) const;
    std::ostream& print(std::ostream& o, const value_t v) const;
    std::string format(const value_t v) const;

  public:
    // Concrete type implementation.
    value_t zero() const;
    value_t unit() const;
    value_t zero(const weight_t& w) const;
    value_t unit(const weight_t& w) const;
    value_t add(value_t l, value_t r) const;
    value_t mul(value_t l, value_t r) const;
    value_t concat(value_t l, value_t r) const;
    /// Implementation details for concat.
    value_t concat(value_t l, value_t r, labels_are_unit) const;
    value_t concat(value_t l, value_t r, labels_are_letters) const;
    value_t concat(value_t l, value_t r, labels_are_words) const;
    value_t star(value_t e) const;
    value_t weight(value_t e, const weight_t& w) const;
    value_t weight(const weight_t& w, value_t e) const;
    value_t transpose(value_t e) const;

  private:
    template <typename Ctx>
    value_t
    atom_(if_lau<Ctx, label_t> v) const;

    template <typename Ctx>
    value_t
    atom_(if_lal<Ctx, letter_t> v) const;

    template <typename Ctx>
    value_t
    atom_(const if_law<Ctx, word_t>& w) const;

    /// Push \a v in \a res, applying associativity if possible.
    /// \param type  the kind of ratexps on which to apply associativity.
    ///              Must be SUM or PROD.
    void gather(ratexps_t& res, rat::exp::type_t type, value_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \param type  the kind of ratexps on which to apply associativity.
    ///              Must be SUM or PROD.
    ratexps_t gather(rat::exp::type_t type, value_t l, value_t r) const;

  private:
    const context_t& ctx_;
  };

} // namespace vcsn

# include <vcsn/core/rat/ratexpset.hxx>

#endif // !VCSN_CORE_RAT_RATEXPSET_HH
