#ifndef VCSN_CORE_RAT_NODE_FWD_HH
# define VCSN_CORE_RAT_NODE_FWD_HH

# include <memory>

namespace vcsn
{
  namespace rat
  {

    class exp;
    using exp_t = std::shared_ptr<exp>;

    template <class Weight>
    class node;

    template <class Weight>
    class inner;

    template <class Weight>
    class leaf;

    template <class Weight>
    class prod;

    template <class Weight>
    class sum;

    template <class Weight>
    class star;

    template <class Weight>
    class one;

    template <class Weight>
    class zero;

    template <class Weight>
    class atom;

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_NODE_FWD_HH
