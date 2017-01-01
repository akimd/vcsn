#pragma once

#include <memory>

#include <vcsn/weightset/fwd.hh>

namespace vcsn
{
  namespace rat
  {
    // identities.hh
    class identities;

    /*---------------.
    | expressions.   |
    `---------------*/

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
    /// `\\e + a`, and never `a + \\e`.
    enum class type_t
    {
      zero,
      one,
      atom,
      add,
      mul,
      ldivide,
      conjunction,
      shuffle,
      infiltrate,
      star,
      transposition,
      lweight,
      rweight,
      complement,
      tuple,
      compose,
    };

    /// Whether is a constant (`\\z` or `\\e`).
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
      return (t == type_t::compose
              || t == type_t::conjunction
              || t == type_t::infiltrate
              || t == type_t::ldivide
              || t == type_t::mul
              || t == type_t::shuffle
              || t == type_t::add);
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
    using compose = variadic<type_t::compose, Context>;

    template <typename Context>
    using conjunction = variadic<type_t::conjunction, Context>;

    template <typename Context>
    using infiltrate = variadic<type_t::infiltrate, Context>;

    template <typename Context>
    using ldivide = variadic<type_t::ldivide, Context>;

    template <typename Context>
    using mul = variadic<type_t::mul, Context>;

    template <typename Context>
    using shuffle = variadic<type_t::shuffle, Context>;

    template <typename Context>
    using add = variadic<type_t::add, Context>;



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
