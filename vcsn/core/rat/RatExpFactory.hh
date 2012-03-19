#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HH_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HH_

# include <string>
# include <list>

# include <core/rat/node.fwd.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class RatExpFactory
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::string weight_str;
      typedef std::list<weight_str *> weight_str_container;
      typedef RatExpNode<weightset_t> node_t;
    public:
      RatExpFactory();
      RatExpFactory(const weightset_t& ws);
    public:
      RatExp* op_mul(RatExp* e);
      RatExp* op_mul(RatExp* l, RatExp* r);
      RatExp* op_add(RatExp* l, RatExp* r);
      RatExp* op_kleene(RatExp* e);

      weight_str_container* op_weight(weight_str* w);
      weight_str_container* op_weight(weight_str* w, weight_str_container* l);
      weight_str_container* op_weight(weight_str_container* l, weight_str* w);

      // RatExp constants' method
      RatExpOne<WeightSet>* op_one();
      RatExpZero<WeightSet>* op_zero();
      // RatExpWord's method
      RatExpWord<WeightSet>* op_word(std::string* w);

      RatExp* op_weight(weight_str_container* w, RatExp* e);
      RatExp* op_weight(RatExp* e, weight_str_container* w);
      node_t* op_weight(LWeightNode<WeightSet>* e, weight_str_container* w);
      node_t* op_weight(weight_str_container* w, LRWeightNode<WeightSet>* e);
      node_t* op_weight(LRWeightNode<WeightSet>* e, weight_str_container* w);

    protected:
      // RatExpConcat's method
      node_t* op_mul(node_t* e);
      node_t* op_mul(node_t* l,  node_t* r);
      // RatExpPlus's method
      node_t* op_add(node_t* l, node_t* r);
      // RatExpKleene's method
      node_t* op_kleene(node_t* e);

    private:
      const WeightSet* ws_;
    };

  } // rat
} // vcsn

# include <core/rat/RatExpFactory.hxx>

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HH_
