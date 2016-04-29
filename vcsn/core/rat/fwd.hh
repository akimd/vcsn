#pragma once

#include <memory>

#include <vcsn/core/rat/identities.hh>
#include <vcsn/misc/type_traits.hh> // detect
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace rat
  {

    /// Base class for the AST.
    class exp;
    using exp_t = std::shared_ptr<const exp>;

    // info.hh
    template <typename ExpSet>
    class info;

    template <typename ExpSet>
    info<ExpSet>
    make_info(const typename ExpSet::value_t& r);

    // printer.hh.
    template <typename ExpSet>
    class printer;

    template <typename ExpSet>
    printer<ExpSet>
    make_printer(const ExpSet& rs, std::ostream& out);

    /// The possible types of expressions.
    ///
    /// They also code the order in which they are sorted by
    /// less, so that, for instance, polynomials always display
    /// `\e + a`, and never `a + \e`.
    enum class type_t
    {
      zero,
      one,
      atom,
      sum,
      prod,
      ldiv,
      conjunction,
      shuffle,
      infiltration,
      star,
      transposition,
      lweight,
      rweight,
      complement,
      tuple,
    };

    /// Whether is a constant (`\z` or `\e`).
    inline constexpr bool is_constant(type_t t)
    {
      return (t == type_t::one
              || t == type_t::zero);
    }

    /// Whether star, complement.
    inline constexpr bool is_unary(type_t t)
    {
      return (t == type_t::complement
              || t == type_t::star
              || t == type_t::transposition);
    }

    /// Whether one of the variadic types.
    inline constexpr bool is_variadic(type_t t)
    {
      return (t == type_t::conjunction
              || t == type_t::infiltration
              || t == type_t::ldiv
              || t == type_t::prod
              || t == type_t::shuffle
              || t == type_t::sum);
    }

    /// Print a expression type.
    std::ostream& operator<<(std::ostream& o, type_t t);

#define DEFINE(Node)             \
    template <typename Context>  \
    class Node

    DEFINE(atom);
    DEFINE(inner);
    DEFINE(leaf);
    DEFINE(node);

    DEFINE(const_visitor);

#undef DEFINE

    /*-----------.
    | constant.  |
    `-----------*/

    template <type_t Type, typename Context>
    class constant;

    template <typename Context>
    using zero = constant<type_t::zero, Context>;

    template <typename Context>
    using one = constant<type_t::one, Context>;

    /*--------.
    | unary.  |
    `--------*/

    template <type_t Type, typename Context>
    class unary;

    template <typename Context>
    using complement = unary<type_t::complement, Context>;

    template <typename Context>
    using star = unary<type_t::star, Context>;

    template <typename Context>
    using transposition = unary<type_t::transposition, Context>;

    /*-----------.
    | variadic.  |
    `-----------*/

    template <type_t Type, typename Context>
    class variadic;

    template <typename Context>
    using prod = variadic<type_t::prod, Context>;

    template <typename Context>
    using conjunction = variadic<type_t::conjunction, Context>;

    template <typename Context>
    using infiltration = variadic<type_t::infiltration, Context>;

    template <typename Context>
    using ldiv = variadic<type_t::ldiv, Context>;

    template <typename Context>
    using shuffle = variadic<type_t::shuffle, Context>;

    template <typename Context>
    using sum = variadic<type_t::sum, Context>;



    /*---------.
    | tuple.   |
    `---------*/

    /// Implementation of nodes of tuple of rational expressions.
    template <typename Context,
              bool Enable = Context::is_lat>
    class tuple;


    /*--------------.
    | weight_node.  |
    `--------------*/

    template <type_t Type, typename Context>
    class weight_node;

    template <typename Context>
    using lweight = weight_node<type_t::lweight, Context>;

    template <typename Context>
    using rweight = weight_node<type_t::rweight, Context>;



    template <typename Context>
    using expression = std::shared_ptr<const node<Context>>;


    // expansionset.hh.
    template <typename ExpSet>
    struct expansionset;

    // expressionset.hh.
    template <typename Context>
    class expressionset_impl;

    // size.hh.
    template <typename ExpSet>
    size_t size(const ExpSet& rs, const typename ExpSet::value_t& r);

  } // namespace rat

  template <typename Context>
  using expressionset = weightset_mixin<rat::expressionset_impl<Context>>;

  template <typename ExpSet>
  using expansionset = rat::expansionset<ExpSet>;

} // namespace vcsn
