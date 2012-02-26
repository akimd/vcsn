#ifndef VCSN_CORE_RAT_EXP_NODE_FWD_HH_
# define VCSN_CORE_RAT_EXP_NODE_FWD_HH_

# include <string>
# include <list>

namespace vcsn {
  namespace rat_exp {

    typedef std::string weight_type;
    typedef std::list<weight_type *> weights_type;

    class exp;

    class concat; //

    class plus; //

    class kleene; //

    class one; //

    class zero; //

    class word;

    class left_weight; //

    class right_weight; //

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_NODE_FWD_HH_
