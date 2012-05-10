#ifndef VCSN_CORE_RAT_NODE_FWD_HH
# define VCSN_CORE_RAT_NODE_FWD_HH

# include <memory>

namespace vcsn
{
  namespace rat
  {

    class exp;
    using exp_t = std::shared_ptr<const exp>;

# define DEFINE(Node)                           \
    template <typename Atom, typename Weight>   \
    class Node

    DEFINE(node);
    DEFINE(leaf);
    DEFINE(zero);
    DEFINE(one);
    DEFINE(atom);
    DEFINE(inner);
    DEFINE(nary);
    DEFINE(sum);
    DEFINE(prod);
    DEFINE(star);

# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_NODE_FWD_HH
