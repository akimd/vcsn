#pragma once

#include <memory> // shared_ptr

#include <vcsn/core/automaton.hh> // Automaton
#include <vcsn/dyn/cast.hh>

namespace vcsn
{
  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    namespace detail
    {
      class automaton_base;
      template <Automaton Aut>
      class automaton_wrapper;
    }
    using automaton = std::shared_ptr<detail::automaton_base>;

    // vcsn/dyn/context.hh.
    namespace detail
    {
      class context_base;
      template <typename Context>
      class context_wrapper;
    }
    /// A dyn::context.
    using context = std::shared_ptr<const detail::context_base>;

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
      class label_base;
      template <typename LabelSet>
      class label_wrapper;
    }
    using label = std::shared_ptr<const detail::label_base>;

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
      class weight_base;
      template <typename WeightSet>
      class weight_wrapper;
    }
    using weight = std::shared_ptr<const detail::weight_base>;
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
