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

    // When taken as a WeightSet, our values.
    using value_t = exp_t*;

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
    virtual zero_t* zero() const;
    virtual one_t* unit() const;
    virtual atom_t* atom(const std::string& w) const;
    virtual exp_t* add(exp_t* l, exp_t* r) const;
    virtual exp_t* mul(exp_t* l, exp_t* r) const;
    virtual exp_t* star(exp_t* e) const;
    virtual exp_t* weight(exp_t* e, std::string* w) const;
    virtual exp_t* weight(std::string* w, exp_t* e) const;

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
    node_t* add(node_t* l, node_t* r) const;
    node_t* mul(node_t* l, node_t* r) const;
    node_t* star(node_t* e) const;
    node_t* weight(node_t* e, const weight_t& w) const;
    node_t* weight(const weight_t& w, node_t* e) const;

  private:
    const genset_t& gs_;
    const weightset_t& ws_;
  };

} // namespace vcsn

# include <vcsn/core/rat/kratexps.hxx>

#endif // !VCSN_CORE_RAT_KRATEXPS_HH
