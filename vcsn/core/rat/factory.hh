#ifndef VCSN_CORE_RAT_RAT_EXP_HH_
# define VCSN_CORE_RAT_RAT_EXP_HH_

# include <string>
# include <list>

# include <core/rat/node.fwd.hh>

namespace vcsn
{

  class factory
  {
  public:
    typedef std::string weight_str;
    typedef std::list<weight_str*> weight_str_container;
    typedef rat::exp exp_t;

    virtual exp_t* op_prod(exp_t* l, exp_t* r) = 0;
    virtual exp_t* op_sum(exp_t* l, exp_t* r) = 0;
    virtual exp_t* op_star(exp_t* e) = 0;
    virtual exp_t* op_one() = 0;
    virtual exp_t* op_zero() = 0;
    virtual exp_t* op_atom(std::string* w) = 0;
    virtual exp_t* op_weight(weight_str_container* w, exp_t* e) = 0;
    virtual exp_t* op_weight(exp_t* e, weight_str_container* w) = 0;

    weight_str_container* op_weight(weight_str* w);
    weight_str_container* op_weight(weight_str* w, weight_str_container* l);
    weight_str_container* op_weight(weight_str_container* l, weight_str* w);
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
  public:
    factory_();
    factory_(const weightset_t& ws);

    virtual exp_t* op_prod(exp_t* l, exp_t* r);
    virtual exp_t* op_sum(exp_t* l, exp_t* r);
    virtual exp_t* op_star(exp_t* e);

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

    one_t* op_one();
    zero_t* op_zero();
    atom_t* op_atom(std::string* w);

    using super_type::op_weight;
    exp_t* op_weight(weight_str_container* w, exp_t* e);
    exp_t* op_weight(exp_t* e, weight_str_container* w);

  protected:
    node_t* op_prod(node_t* l, node_t* r);
    node_t* op_sum(node_t* l, node_t* r);
    node_t* op_star(node_t* e);
    node_t* op_weight(leaf_t* e, weight_str_container* w);
    node_t* op_weight(weight_str_container* w, inner_t* e);
    node_t* op_weight(inner_t* e, weight_str_container* w);

  private:
    const weightset_t* ws_;
  };

} // namespace vcsn

# include <core/rat/factory.hxx>

#endif // !VCSN_CORE_RAT_RAT_EXP_HH_
