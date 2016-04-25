#pragma once

#include <memory> // shared_ptr

#include <vcsn/dyn/cast.hh>

namespace vcsn
{
  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    namespace detail
    {
      class automaton;
    }
    using automaton = detail::automaton;

    // vcsn/dyn/context.hh.
    namespace detail
    {
      class context;
    }
    /// A dyn::context.
    using context = detail::context;

    // vcsn/dyn/value.hh.
    namespace detail
    {
      struct expansion_tag;
      struct expression_tag;
      struct label_tag;
      struct polynomial_tag;
      struct weight_tag;
      template <typename tag>
      class value;
    }
    using expansion = detail::value<detail::expansion_tag>;
    using expression = detail::value<detail::expression_tag>;
    using label = detail::value<detail::label_tag>;
    using polynomial = detail::value<detail::polynomial_tag>;
    using weight = detail::value<detail::weight_tag>;

  }

  // vcsn/dyn/type-ast.hh.
  namespace ast
  {
    class ast_node;
    class automaton;
    class context;
    class expansionset;
    class expressionset;
    class genset;
    class letterset;
    class nullableset;
    class oneset;
    class other;
    class polynomialset;
    class tuple;
    class tupleset;
    class weightset;
    class wordset;
  }
} // namespace vcsn
