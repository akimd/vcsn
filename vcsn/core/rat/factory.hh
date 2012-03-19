#ifndef VCSN_CORE_RAT_RAT_EXP_HH_
# define VCSN_CORE_RAT_RAT_EXP_HH_

# include <string>
# include <list>

# include <core/rat/node.fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class factory
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::string weight_str;
      typedef std::list<weight_str *> weight_str_container;
      typedef node<weightset_t> node_t;
    public:
      factory();
      factory(const weightset_t& ws);
    public:
      exp* op_mul(exp* e);
      exp* op_mul(exp* l, exp* r);
      exp* op_add(exp* l, exp* r);
      exp* op_kleene(exp* e);

      weight_str_container* op_weight(weight_str* w);
      weight_str_container* op_weight(weight_str* w, weight_str_container* l);
      weight_str_container* op_weight(weight_str_container* l, weight_str* w);

      // exp constants' method
      one<WeightSet>* op_one();
      zero<WeightSet>* op_zero();
      // word's method
      word<WeightSet>* op_word(std::string* w);

      exp* op_weight(weight_str_container* w, exp* e);
      exp* op_weight(exp* e, weight_str_container* w);
      node_t* op_weight(left_weighted<WeightSet>* e, weight_str_container* w);
      node_t* op_weight(weight_str_container* w, weighted<WeightSet>* e);
      node_t* op_weight(weighted<WeightSet>* e, weight_str_container* w);

    protected:
      // concat's method
      node_t* op_mul(node_t* e);
      node_t* op_mul(node_t* l,  node_t* r);
      // plus's method
      node_t* op_add(node_t* l, node_t* r);
      // kleene's method
      node_t* op_kleene(node_t* e);

    private:
      const WeightSet* ws_;
    };

  } // rat
} // vcsn

# include <core/rat/factory.hxx>

#endif // !VCSN_CORE_RAT_RAT_EXP_HH_
