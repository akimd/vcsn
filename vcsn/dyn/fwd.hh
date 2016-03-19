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

    // vcsn/dyn/expansion.hh.
    namespace detail
    {
      class expansion_base;
      template <typename ExpansionSet>
      class expansion_wrapper;
    }
    using expansion = std::shared_ptr<const detail::expansion_base>;

    // vcsn/dyn/label.hh.
    namespace detail
    {
      class label;
    }
    using label = detail::label;

    // vcsn/dyn/polynomial.hh.
    namespace detail
    {
      class polynomial_base;
      template <typename PolynomialSet>
      class polynomial_wrapper;
    }
    using polynomial = std::shared_ptr<const detail::polynomial_base>;

    // vcsn/dyn/expression.hh.
    namespace detail
    {
      class expression_base;
      template <typename ExpSet>
      class expression_wrapper;
    }
    using expression = std::shared_ptr<detail::expression_base>;

    // vcsn/dyn/weight.hh.
    namespace detail
    {
      class weight;
    }
    using weight = detail::weight;
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
