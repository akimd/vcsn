#ifndef VCSN_CORE_RAT_KRATEXPSET_HH
# define VCSN_CORE_RAT_KRATEXPSET_HH

# include <string>
# include <list>

# include <vcsn/core/kind.hh>
# include <vcsn/core/rat/kratexp.hh>
# include <vcsn/core/rat/printer.hh>

namespace vcsn
{

  template <typename Context>
  class kratexpset
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;
    using kind_t = typename context_t::kind_t;
    using genset_ptr = typename context_t::genset_ptr;
    using weightset_ptr = typename context_t::weightset_ptr;
    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using label_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;
    /// Type of printer visitor.
    using printer_t = rat::printer<context_t>;
    /// Type of kratexps.
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
    using kratexps_t = typename node_t::kratexps_t;

    /// The value this is a set of: typeful shared pointers.
    using value_t = typename node_t::value_t;

  public:
    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    kratexpset(const context_t& ctx)
      : ctx_(ctx)
    {}

    const context_t& context() const
    {
      return ctx_;
    }

    const genset_ptr& genset() const
    {
      return ctx_.genset();
    }

    const weightset_ptr& weightset() const
    {
      return ctx_.weightset();
    }

    auto atom(const label_t& v) const
      -> value_t;

    template <typename Kind>
    auto
    atom_(const letter_t& v) const
      -> typename std::enable_if<std::is_same<Kind, labels_are_letters>::value,
                                 value_t>::type;

    template <typename Kind>
    auto
    atom_(const word_t& w) const
      -> typename std::enable_if<std::is_same<Kind, labels_are_words>::value,
                                 value_t>::type;

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
    std::string format(const value_t v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }

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
    value_t concat(value_t l, value_t r, labels_are_letters) const;
    value_t concat(value_t l, value_t r, labels_are_words) const;
    value_t star(value_t e) const;
    value_t weight(value_t e, const weight_t& w) const;
    value_t weight(const weight_t& w, value_t e) const;

    /// Push \a v in \a res, applying associativity if possible.
    /// \param type  the kind of kratexps on which to apply associativity.
    ///              Must be SUM or PROD.
    void gather(kratexps_t& res, rat::exp::type_t type, value_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \param type  the kind of kratexps on which to apply associativity.
    ///              Must be SUM or PROD.
    kratexps_t gather(rat::exp::type_t type, value_t l, value_t r) const;

  private:
    const context_t& ctx_;
  };

} // namespace vcsn

# include <vcsn/core/rat/kratexpset.hxx>

#endif // !VCSN_CORE_RAT_KRATEXPSET_HH
