#ifndef VCSN_CORE_RAT_RAT_EXP_HH_
# define VCSN_CORE_RAT_RAT_EXP_HH_

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{

  class factory
  {
  public:
    typedef std::string weight_str;
    typedef std::list<weight_str*> weight_str_container;
    typedef rat::exp exp_t;

    virtual exp_t* mul(exp_t* l, exp_t* r) const = 0;
    virtual exp_t* add(exp_t* l, exp_t* r) const = 0;
    virtual exp_t* star(exp_t* e) const = 0;
    virtual exp_t* unit() const = 0;
    virtual exp_t* zero() const = 0;
    virtual exp_t* atom(std::string* w) const = 0;
    virtual exp_t* weight(weight_str_container* w, exp_t* e) const = 0;
    virtual exp_t* weight(exp_t* e, weight_str_container* w) const = 0;

    weight_str_container* weight(weight_str* w) const;
    weight_str_container* weight(weight_str* w, weight_str_container* l) const;
    weight_str_container* weight(weight_str_container* l, weight_str* w) const;
  };

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
    factory_();
    factory_(const weightset_t& ws);

    virtual exp_t* mul(exp_t* l, exp_t* r) const;
    virtual exp_t* add(exp_t* l, exp_t* r) const;
    virtual exp_t* star(exp_t* e) const;

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

    using super_type::weight;
    exp_t* weight(weight_str_container* w, exp_t* e) const;
    exp_t* weight(exp_t* e, weight_str_container* w) const;

  protected:
    node_t* mul(node_t* l, node_t* r) const;
    node_t* add(node_t* l, node_t* r) const;
    node_t* star(node_t* e) const;
    node_t* weight(leaf_t* e, weight_str_container* w) const;
    node_t* weight(weight_str_container* w, inner_t* e) const;
    node_t* weight(inner_t* e, weight_str_container* w) const;

  private:
    const weightset_t* ws_;
  };

} // namespace vcsn

# include <vcsn/core/rat/factory.hxx>

#endif // !VCSN_CORE_RAT_RAT_EXP_HH_
