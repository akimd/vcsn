#ifndef VCSN_CORE_RAT_FACTORY__HH
# define VCSN_CORE_RAT_FACTORY__HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>
# include <vcsn/core/rat/factory.hh>

namespace vcsn
{

  template <typename Alphabet, typename WeightSet>
  class factory_ : public factory
  {
  public:
    typedef Alphabet alphabet_t;
    typedef WeightSet weightset_t;
    typedef factory super_type;
    typedef typename weightset_t::value_t weight_t;
    typedef rat::node<weight_t> node_t;
    typedef rat::exp exp_t;

    // When taken as a WeightSet, our values.
    typedef exp_t* value_t;

  public:
    /// Constructor.
    /// \param a    the alphabet for the labels.
    /// \param ws   the type of the weights (e.g., "bool", "int").
    factory_(const alphabet_t& a, const weightset_t& ws);
    /// Construct with \a t as weight-set.
    /// \param a    the alphabet for the labels.
    /// \param t    \a t must be castable to weightset_t.
    template <typename T>
    factory_(const alphabet_t& a, const T& t);

    const alphabet_t& alphabet() const
    {
      return a_;
    }

    const weightset_t& weightset() const
    {
      return ws_;
    }

    // exp constants' method
#define DEFINE(Type)                            \
    typedef rat::Type<weight_t> Type ## _t
    DEFINE(leaf);
    DEFINE(zero);
    DEFINE(one);
    DEFINE(atom);
    DEFINE(inner);
    DEFINE(sum);
    DEFINE(prod);
    DEFINE(star);
#undef DEFINE

    // Specialization from factory.
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
    bool show_unit() const;
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
    const alphabet_t& a_;
    const weightset_t& ws_;
  };

} // namespace vcsn

# include <vcsn/core/rat/factory_.hxx>

#endif // !VCSN_CORE_RAT_FACTORY__HH
