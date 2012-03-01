#ifndef VCSN_CORE_RAT_EXP_RAT_EXP_HH_
# define VCSN_CORE_RAT_EXP_RAT_EXP_HH_

# include <string>

# include <core/rat_exp/node.fwd.hh>

namespace vcsn {
  namespace rat_exp {

    template<class WeightSet>
    class RatExp
    {
    public:
      concat *op_mul(exp *e);
      concat *op_mul(exp *l,    exp *r);
      concat *op_mul(exp *l, concat *r);
      concat *op_mul(concat *l, exp *r);

      plus *op_add(exp *l,  exp *r);
      plus *op_add(exp *l,  plus *r);
      plus *op_add(plus *l, exp *r);

      kleene *op_kleene(exp *e);

      one  *op_one();
      zero *op_zero();

      left_weight  *op_weight(weights_type *l,  exp *r);
      right_weight *op_weight(exp *l,          weights_type *r);

      word *op_word(std::string *w);

      weights_type *op_weight(weight_type *w);
      weights_type *op_weight(weight_type *w, weights_type *l);
      weights_type *op_weight(weights_type *l, weight_type *w);
    // private:
    //   WeightSet weight_set_;
    };

  } // !rat_exp
} // !vcsn

#include <core/rat_exp/rat-exp.hxx>

#endif // !VCSN_CORE_RAT_EXP_RAT_EXP_HH_
