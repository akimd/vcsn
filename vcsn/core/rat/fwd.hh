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
    ///
    /// They also code the order in which they are sorted by
    /// less_than, so that, for instance, polynomials always display
    /// '\e + a', and never 'a + \e'.
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
      lweight = 8,
      rweight = 9,
      complement = 10,
    };

# define DEFINE(Node)                           \
    template <typename Label, typename Weight>  \
    class Node

    DEFINE(node);
    DEFINE(leaf);
    DEFINE(atom);
    DEFINE(inner);

    DEFINE(const_visitor);

# undef DEFINE

    template <type_t Type, typename Label, typename Weight>
    class unary;

    template <typename Label, typename Weight>
    using complement = unary<type_t::complement, Label, Weight>;

    template <typename Label, typename Weight>
    using star = unary<type_t::star, Label, Weight>;


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
    class weight_node;

    template <typename Label, typename Weight>
    using lweight = weight_node<type_t::lweight, Label, Weight>;
    template <typename Label, typename Weight>
    using rweight = weight_node<type_t::rweight, Label, Weight>;

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
