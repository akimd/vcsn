#ifndef VCSN_CORE_RAT_FACTORY__HH
# define VCSN_CORE_RAT_FACTORY__HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>
# include <vcsn/core/rat/factory.hh>

namespace vcsn
{

  template <class WeightSet>
  class factory_ : public factory
  {
  public:
    typedef WeightSet weightset_t;
    typedef factory super_type;
    typedef typename weightset_t::value_t weight_t;
    typedef rat::node<weight_t> node_t;
    typedef rat::exp exp_t;

    // When taken as a WeightSet, our values.
    typedef exp_t* value_t;

  public:
    factory_(const weightset_t& ws);

    virtual exp_t* mul(exp_t* l, exp_t* r) const;
    virtual exp_t* add(exp_t* l, exp_t* r) const;
    virtual exp_t* star(exp_t* e) const;
    virtual exp_t* weight(exp_t* e, std::string* w) const;
    virtual exp_t* weight(std::string* w, exp_t* e) const;

    // exp constants' method
#define DEFINE(Type)                            \
    typedef rat::Type<weight_t> Type ## _t
    DEFINE(one);
    DEFINE(zero);
    DEFINE(atom);
    DEFINE(leaf);
    DEFINE(inner);
    DEFINE(star);
    DEFINE(prod);
    DEFINE(sum);
#undef DEFINE

    one_t* unit() const;
    zero_t* zero() const;
    atom_t* atom(std::string* w) const;

    bool is_zero(value_t v) const;
    bool is_unit(value_t v) const;
    bool show_unit() const;
    value_t conv(const std::string& s) const;

    virtual
    std::ostream&
    print(std::ostream& o, const value_t v) const;

  protected:
    node_t* mul(node_t* l, node_t* r) const;
    node_t* add(node_t* l, node_t* r) const;
    node_t* star(node_t* e) const;
    node_t* weight(node_t* e, const weight_t& w) const;
    node_t* weight(const weight_t& w, node_t* e) const;

  private:
    const weightset_t* ws_;
  };

} // namespace vcsn

# include <vcsn/core/rat/factory_.hxx>

#endif // !VCSN_CORE_RAT_FACTORY__HH
