#ifndef VCSN_CORE_RAT_FWD_HH
# define VCSN_CORE_RAT_FWD_HH

# include <memory>

namespace vcsn
{
  namespace rat
  {

    /// Base class for the AST.
    class exp;
    using exp_t = std::shared_ptr<const exp>;

    // info.hh
    template <typename Context>
    class info;

    // printer.hh.
    template <typename Context>
    class printer;

# define DEFINE(Node)                           \
    template <typename Label, typename Weight>  \
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

    DEFINE(const_visitor);

# undef DEFINE

    template <typename Label, typename Weight>
    using ratexp = std::shared_ptr<const node<Label, Weight>>;
  } // namespace rat

  // ratexpset.hh.
  template <typename Context>
  class ratexpset;

} // namespace vcsn

#endif // !VCSN_CORE_RAT_FWD_HH
