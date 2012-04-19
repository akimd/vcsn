#ifndef VCSN_CORE_RAT_KRATEXPS_HH
# define VCSN_CORE_RAT_KRATEXPS_HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>
# include <vcsn/core/rat/abstract_kratexps.hh>

namespace vcsn
{

  template <typename GenSet, typename WeightSet>
  class kratexps : public abstract_kratexps
  {
  public:
    using genset_t = GenSet;
    using weightset_t = WeightSet;
    using super_type = abstract_kratexps;
    using weight_t = typename weightset_t::value_t;
    using node_t = rat::node<weight_t>;
    using exp_t = rat::exp;

    // When taken as a WeightSet, our (abstract) value type.
    using value_t = exp_t*;
    // Concrete value type.
    using kvalue_t = node_t*;

  public:
    /// Constructor.
    /// \param a    the generator set for the labels.
    /// \param ws   the type of the weights (e.g., "bool", "int").
    kratexps(const genset_t& gs, const weightset_t& ws);
    /// Construct with \a t as weight-set.
    /// \param a    the generator set for the labels.
    /// \param t    \a t must be castable to weightset_t.
    template <typename T>
    kratexps(const genset_t& gs, const T& t);

    const genset_t& genset() const
    {
      return gs_;
    }

    const weightset_t& weightset() const
    {
      return ws_;
    }

    // exp constants' method
#define DEFINE(Type)                            \
    using Type ## _t = rat::Type<weight_t>
    DEFINE(leaf);
    DEFINE(zero);
    DEFINE(one);
    DEFINE(atom);
    DEFINE(inner);
    DEFINE(sum);
    DEFINE(prod);
    DEFINE(star);
#undef DEFINE

    // Specialization from abstract_kratexps.
    virtual kvalue_t zero() const;
    virtual kvalue_t unit() const;
    virtual kvalue_t atom(const std::string& w) const;
    virtual value_t add(value_t l, value_t r) const;
    virtual value_t mul(value_t l, value_t r) const;
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
    kvalue_t star(kvalue_t e) const;
    kvalue_t weight(kvalue_t e, const weight_t& w) const;
    kvalue_t weight(const weight_t& w, kvalue_t e) const;

  private:
    const genset_t& gs_;
    const weightset_t& ws_;
  };

} // namespace vcsn

# include <vcsn/core/rat/kratexps.hxx>

#endif // !VCSN_CORE_RAT_KRATEXPS_HH
