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

    /// The possible types of ratexps.
    enum class type_t
    {
      zero = 0,
      one  = 1,
      atom = 2,
      sum  = 3,
      prod = 4,
      intersection = 5,
      shuffle = 6,
      star = 7,
    };

# define DEFINE(Node)                           \
    template <typename Label, typename Weight>  \
    class Node

    DEFINE(node);
    DEFINE(leaf);
    DEFINE(atom);
    DEFINE(inner);
    DEFINE(star);

    DEFINE(const_visitor);

# undef DEFINE

    template <type_t Type, typename Label, typename Weight>
    class nary;

    template <typename Label, typename Weight>
    using prod = nary<type_t::prod, Label, Weight>;

    template <typename Label, typename Weight>
    using intersection = nary<type_t::intersection, Label, Weight>;

    template <typename Label, typename Weight>
    using shuffle = nary<type_t::shuffle, Label, Weight>;

    template <typename Label, typename Weight>
    using sum = nary<type_t::sum, Label, Weight>;

    template <type_t Type, typename Label, typename Weight>
    class constant;

    template <typename Label, typename Weight>
    using zero = constant<type_t::zero, Label, Weight>;

    template <typename Label, typename Weight>
    using one = constant<type_t::one, Label, Weight>;

    template <typename Label, typename Weight>
    using ratexp = std::shared_ptr<const node<Label, Weight>>;
  } // namespace rat

  // ratexpset.hh.
  template <typename Context>
  class ratexpset;

} // namespace vcsn

#endif // !VCSN_CORE_RAT_FWD_HH
