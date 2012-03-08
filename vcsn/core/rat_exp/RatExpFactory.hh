#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HH_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HH_

# include <string>

# include <core/rat_exp/node.fwd.hh>

namespace vcsn {
  namespace rat_exp {

    template<class WeightSet>
    class RatExpFactory
    {
    public:
      RatExp *op_mul(RatExp *e);
      RatExp *op_mul(RatExp *l, RatExp *r);
      RatExp *op_add(RatExp *l, RatExp *r);
      RatExp *op_kleene(RatExp *e);

      weights_type *op_weight(weight_type *w);
      weights_type *op_weight(weight_type *w, weights_type *l);
      weights_type *op_weight(weights_type *l, weight_type *w);

      // RatExp constants' method
      RatExpOne<WeightSet>  *op_one();
      RatExpZero<WeightSet> *op_zero();
      // RatExpWord's method
      RatExpWord<WeightSet> *op_word(std::string *w);

      RatExp *op_weight(weights_type *, RatExp *);
      RatExp *op_weight(RatExp *, weights_type *);
      LWeightNode<WeightSet> *op_weight(weights_type *w,
                                        LWeightNode<WeightSet> *e);
      LRWeightNode<WeightSet> *op_weight(weights_type *w,
                                         LRWeightNode<WeightSet> *e);
      LRWeightNode<WeightSet> *op_weight(LRWeightNode<WeightSet> *e,
                                         weights_type *w);

      RatExp *cleanNode(RatExp *e);
    protected:
      // RatExpConcat's method
      RatExpConcat<WeightSet> *op_mul(RatExpNode<WeightSet> *e);
      RatExpConcat<WeightSet> *op_mul(RatExpNode<WeightSet> *l,  RatExpNode<WeightSet> *r);
      // RatExpPlus's method
      RatExpPlus<WeightSet> *op_add(RatExpNode<WeightSet> *l, RatExpNode<WeightSet> *r);
      // RatExpKleene's method
      RatExpKleene<WeightSet> *op_kleene(RatExpNode<WeightSet> *e);

    };

  } // !rat_exp
} // !vcsn

#include <core/rat_exp/RatExpFactory.hxx>

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HH_
