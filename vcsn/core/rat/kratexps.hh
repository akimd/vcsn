#ifndef VCSN_CORE_RAT_KRATEXPS_HH
# define VCSN_CORE_RAT_KRATEXPS_HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.hh>
# include <vcsn/core/rat/kind.hh>
# include <vcsn/core/rat/abstract_kratexps.hh>
# include <vcsn/core/rat/printer.hh>

namespace vcsn
{

  template <typename Context,
            typename Kind = atoms_are_letters>
  class kratexps : public abstract_kratexps
  {
  public:
    using context_t = Context;
    using genset_t = typename context_t::genset_t;
    using weightset_t = typename context_t::weightset_t;
    using kind_t = Kind;
    using super_type = abstract_kratexps;
    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using atom_value_t = typename atom_trait<kind_t, genset_t>::type;
    using weight_t = typename weightset_t::value_t;
    /// Type of printer visitor.
    using printer_t = rat::printer<genset_t, weightset_t, kind_t>;
    /// Type of nodes.
# define DEFINE(Type)                                           \
    using Type ## _t = rat::Type<atom_value_t, weight_t>
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
    using nodes_t = typename node_t::nodes_t;

    /// When taken as a WeightSet, our (abstract) value type.
    using value_t = rat::exp_t;
    /// Concrete value type.
    using kvalue_t = typename node_t::kvalue_t;

  public:
    /// Constructor.
    /// \param a    the generator set for the labels.
    /// \param ws   the type of the weights (e.g., "bool", "int").
    kratexps(const genset_t& gs, const weightset_t& ws)
      : super_type()
      , gs_(gs)
      , ws_(ws)
    {}
    /// Construct with \a t as weight-set.
    /// \param a    the generator set for the labels.
    /// \param t    \a t must be castable to weightset_t.
    template <typename T>
    kratexps(const genset_t& gs, const T& t)
      : kratexps(gs, dynamic_cast<const weightset_t&>(t))
    {}

    const genset_t& genset() const
    {
      return gs_;
    }

    const weightset_t& weightset() const
    {
      return ws_;
    }

    // Specialization from abstract_kratexps.
    virtual value_t zero() const;
    virtual value_t unit() const;

    virtual value_t atom(const word_t& w) const;
    template <typename K>
    typename std::enable_if<std::is_same<K, atoms_are_letters>::value,
                            value_t>::type
    atom_(const word_t& w) const;

    template <typename K>
    typename std::enable_if<std::is_same<K, atoms_are_words>::value,
                            value_t>::type
    atom_(const word_t& w) const;


    virtual value_t add(value_t l, value_t r) const;
    virtual value_t mul(value_t l, value_t r) const;
    /// When concatenating two atoms, possibly make a single one,
    /// or make the product.
    virtual value_t concat(value_t l, value_t r) const;
    virtual value_t star(value_t e) const;
    virtual value_t weight(value_t e, std::string* w) const;
    virtual value_t weight(std::string* w, value_t e) const;

    // When used as WeightSet for automata.
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
    virtual std::ostream& print(std::ostream& o, const value_t v) const;

  protected:
    // Concrete type implementation.
    kvalue_t add(kvalue_t l, kvalue_t r) const;
    kvalue_t mul(kvalue_t l, kvalue_t r) const;
    value_t concat(value_t l, value_t r, atoms_are_letters) const;
    value_t concat(value_t l, value_t r, atoms_are_words) const;
    kvalue_t star(kvalue_t e) const;
    kvalue_t weight(kvalue_t e, const weight_t& w) const;
    kvalue_t weight(const weight_t& w, kvalue_t e) const;

    /// Push \a v in \a res, applying associativity if possible.
    /// \param type  the kind of nodes on which to apply associativity.
    ///              Must be SUM or PROD.
    void gather(nodes_t& res, rat::exp::type_t type, kvalue_t v) const;

    /// A list denoting the gathering of \a l and \a r, applying
    /// associativity if possible.
    /// \param type  the kind of nodes on which to apply associativity.
    ///              Must be SUM or PROD.
    nodes_t gather(rat::exp::type_t type, kvalue_t l, kvalue_t r) const;

  private:
    const genset_t& gs_;
    const weightset_t& ws_;
  };

} // namespace vcsn

# include <vcsn/core/rat/kratexps.hxx>

#endif // !VCSN_CORE_RAT_KRATEXPS_HH
