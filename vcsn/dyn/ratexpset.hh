#ifndef VCSN_DYN_RATEXPSET_HH
# define VCSN_DYN_RATEXPSET_HH

# include <string>
# include <list>
# include <memory>  // shared_ptr

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{
namespace dyn
{
namespace detail
{

  /// Abstract wrapper around a (typeful) ratexpset.
  ///
  /// Use it when you want to avoid depending on the ratexpset
  /// parameters (e.g., from a parser).  To use it, actually create a
  /// derived class (concrete_abstract_ratexpset) with the given
  /// parameters, but handle as a reference to an abstract_ratexpset.
  class abstract_ratexpset
  {
  public:
    using context_t = dyn::detail::abstract_context;
    using value_t = rat::exp_t;

    virtual value_t zero() const = 0;
    virtual value_t one() const = 0;
    /// Throws std::domain_error if w is not a valid label_t.
    virtual value_t atom(const std::string& w) const = 0;
    virtual value_t add(value_t l, value_t r) const = 0;
    /// Explicit product.
    virtual value_t mul(value_t l, value_t r) const = 0;
    /// Implicit product.  If both \a l and \a r are weightless words,
    /// produce a new word that concatenates them.  Otherwise, use \a mul.
    virtual value_t concat(value_t l, value_t r) const = 0;
    virtual value_t star(value_t e) const = 0;
    virtual value_t weight(const std::string& w, value_t e) const = 0;
    virtual value_t weight(value_t e, const std::string& w) const = 0;

    virtual dyn::ratexp make_ratexp(const value_t& v) const = 0;

    /// Parsing.
    virtual value_t conv(const std::string& s) const = 0;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
    std::string format(const value_t v) const;
  };

  /// Wrapper around a ratexpset.
  template <typename Context>
  class concrete_abstract_ratexpset : public abstract_ratexpset
  {
  public:
    using context_t = Context;
    using ratexpset_t = vcsn::ratexpset<Context>;
    using super_type = abstract_ratexpset;
    using label_t = typename context_t::label_t;
    using weight_t = typename context_t::weight_t;
    using value_t = typename super_type::value_t;
    /// Concrete value type.
    using node_t = typename context_t::node_t;

    /// Constructor.
    /// \param ctx    the generator set for the labels, and the weight set.
    concrete_abstract_ratexpset(const context_t& ctx);

    /// From weak to strong typing.
    std::shared_ptr<const node_t>
    down(const value_t& v) const;

    /// From string, to typed weight.
    weight_t down(const std::string& w) const;

    virtual dyn::ratexp make_ratexp(const value_t& v) const override;

    /*------------------------------------------.
    | Specializations from abstract_ratexpset.  |
    `------------------------------------------*/

    virtual value_t zero() const override;
    virtual value_t one() const override;

    virtual value_t atom(const std::string& w) const override;

    virtual value_t add(value_t l, value_t r) const override;

    virtual value_t mul(value_t l, value_t r) const override;

    /// When concatenating two atoms, possibly make a single one,
    /// or make the product.
    virtual value_t concat(value_t l, value_t r) const override;

    virtual value_t star(value_t v) const override;

    virtual value_t weight(const std::string& w, value_t v) const override;

    virtual value_t weight(value_t v, const std::string& w) const override;

    /// Parsing.
    virtual value_t conv(const std::string& s) const override;

    virtual std::ostream& print(std::ostream& o, value_t v) const override;

  private:
    ratexpset_t rs_;
  };
} // namespace detail
} // namespace dyn
} // namespace vcsn

# include <vcsn/dyn/ratexpset.hxx>

#endif // !VCSN_DYN_RATEXPSET_HH
