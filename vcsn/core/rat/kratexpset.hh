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
    using atom_value_t = typename context_t::label_t;
    using weight_t = typename weightset_t::value_t;
    /// Type of printer visitor.
    using printer_t = rat::printer<context_t>;
    /// Type of kratexps.
# define DEFINE(Type)                                           \
    using Type ## _t = rat::Type<atom_value_t, weight_t>
    DEFINE(kratexp);
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
    using kratexps_t = typename kratexp_t::kratexps_t;

    /// Concrete value type.
    using value_t = typename kratexp_t::kvalue_t;
    using kvalue_t = typename kratexp_t::kvalue_t;

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

    kvalue_t atom(const word_t& w) const;
    template <typename K>
    typename std::enable_if<std::is_same<K, labels_are_letters>::value,
                            kvalue_t>::type
    atom_(const word_t& w) const;

    template <typename K>
    typename std::enable_if<std::is_same<K, labels_are_words>::value,
                            kvalue_t>::type
    atom_(const word_t& w) const;

    // When used as WeightSet for automata.
    bool is_zero(kvalue_t v) const;
    bool is_unit(kvalue_t v) const;

    static constexpr bool show_unit()
    {
      return false;
    }

    static constexpr bool is_positive_semiring()
    {
      return weightset_t::is_positive_semiring();
    }

    kvalue_t conv(const std::string& s) const;
    std::ostream& print(std::ostream& o, const kvalue_t v) const;

  public:
    // Concrete type implementation.
    kvalue_t zero() const;
    kvalue_t unit() const;
    kvalue_t zero(const weight_t& w) const;
    kvalue_t unit(const weight_t& w) const;
    kvalue_t add(kvalue_t l, kvalue_t r) const;
    kvalue_t mul(kvalue_t l, kvalue_t r) const;
    kvalue_t concat(kvalue_t l, kvalue_t r) const;
    kvalue_t concat(kvalue_t l, kvalue_t r, labels_are_letters) const;
    kvalue_t concat(kvalue_t l, kvalue_t r, labels_are_words) const;
    kvalue_t star(kvalue_t e) const;
    kvalue_t weight(kvalue_t e, const weight_t& w) const;
    kvalue_t weight(const weight_t& w, kvalue_t e) const;

    /// Push \a v in \a res, applying associativity if possible.
    /// \param type  the kind of kratexps on which to apply associativity.
    ///              Must be SUM or PROD.
    void gather(kratexps_t& res, rat::exp::type_t type, kvalue_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \param type  the kind of kratexps on which to apply associativity.
    ///              Must be SUM or PROD.
    kratexps_t gather(rat::exp::type_t type, kvalue_t l, kvalue_t r) const;

  private:
    const context_t& ctx_;
  };

} // namespace vcsn

# include <vcsn/core/rat/kratexpset.hxx>

#endif // !VCSN_CORE_RAT_KRATEXPSET_HH
