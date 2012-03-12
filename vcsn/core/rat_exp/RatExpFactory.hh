#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HH_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HH_

# include <string>

# include <core/rat_exp/node.fwd.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    class RatExpFactory
    {
    public:
      RatExp *op_mul(RatExp *e);
      RatExp *op_mul(RatExp* l, RatExp* r);
      RatExp *op_add(RatExp* l, RatExp* r);
      RatExp *op_kleene(RatExp* e);

      weights_type *op_weight(weight_type* w);
      weights_type *op_weight(weight_type* w, weights_type* l);
      weights_type *op_weight(weights_type* l, weight_type* w);

      // RatExp constants' method
      RatExpOne<WeightSet>  *op_one();
      RatExpZero<WeightSet> *op_zero();
      // RatExpWord's method
      RatExpWord<WeightSet> *op_word(std::string* w);

      RatExp *op_weight(weights_type* w, RatExp* e);
      RatExp *op_weight(RatExp* e, weights_type* w);
      RatExpNode<WeightSet> *op_weight(LWeightNode<WeightSet>* e, weights_type* w);
      RatExpNode<WeightSet> *op_weight(weights_type* w, LRWeightNode<WeightSet>* e);
      RatExpNode<WeightSet> *op_weight(LRWeightNode<WeightSet>* e, weights_type* w);

      RatExp *clean_node(RatExp* e);
    protected:
      // RatExpConcat's method
      RatExpNode<WeightSet> *op_mul(RatExpNode<WeightSet>* e);
      RatExpNode<WeightSet> *op_mul(RatExpNode<WeightSet>* l,  RatExpNode<WeightSet>* r);
      // RatExpPlus's method
      RatExpNode<WeightSet> *op_add(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r);
      // RatExpKleene's method
      RatExpNode<WeightSet> *op_kleene(RatExpNode<WeightSet>* e);

    };

  } // !rat_exp
} // !vcsn

# include <core/rat_exp/RatExpFactory.hxx>

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HH_
