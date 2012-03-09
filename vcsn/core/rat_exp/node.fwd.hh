#ifndef VCSN_CORE_RAT_EXP_NODE_FWD_HH_
# define VCSN_CORE_RAT_EXP_NODE_FWD_HH_

# include <string>
# include <list>

namespace vcsn
{
  namespace rat_exp
  {

    typedef std::string weight_type;
    typedef std::list<weight_type*> weights_type;

    class RatExp;

    template <class WeightSet>
    class RatExpNode;

    template <class WeightSet>
    class RatExpConcat;

    template <class WeightSet>
    class RatExpPlus;

    template <class WeightSet>
    class RatExpKleene;

    template <class WeightSet>
    class RatExpOne;

    template <class WeightSet>
    class RatExpZero;

    template <class WeightSet>
    class RatExpWord;

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_NODE_FWD_HH_
