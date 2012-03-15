#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HH_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HH_

# include <string>
# include <list>

# include <core/rat_exp/node.fwd.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    class RatExpFactory
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::string weight_str;
      typedef std::list<weight_str *> weight_str_container;
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
      RatExpNode<WeightSet>* op_weight(LWeightNode<WeightSet>* e, weight_str_container* w);
      RatExpNode<WeightSet>* op_weight(weight_str_container* w, LRWeightNode<WeightSet>* e);
      RatExpNode<WeightSet>* op_weight(LRWeightNode<WeightSet>* e, weight_str_container* w);

      RatExp* clean_node(RatExp* e);
    protected:
      // RatExpConcat's method
      RatExpNode<WeightSet>* op_mul(RatExpNode<WeightSet>* e);
      RatExpNode<WeightSet>* op_mul(RatExpNode<WeightSet>* l,  RatExpNode<WeightSet>* r);
      // RatExpPlus's method
      RatExpNode<WeightSet>* op_add(RatExpNode<WeightSet>* l, RatExpNode<WeightSet>* r);
      // RatExpKleene's method
      RatExpNode<WeightSet>* op_kleene(RatExpNode<WeightSet>* e);

    private:
      const WeightSet* ws_;
    };

  } // rat_exp
} // vcsn

# include <core/rat_exp/RatExpFactory.hxx>

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HH_
