#ifndef VCSN_CORE_RAT_NODE_FWD_HH_
# define VCSN_CORE_RAT_NODE_FWD_HH_

namespace vcsn
{
  namespace rat
  {

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

  } // rat
} // vcsn

#endif // !VCSN_CORE_RAT_NODE_FWD_HH_
