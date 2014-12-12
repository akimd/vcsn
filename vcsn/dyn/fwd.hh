#pragma once

#include <memory> // shared_ptr

#include <vcsn/misc/export.hh> // LIBVCSN_API
#include <vcsn/misc/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// A dynamic_cast in debug mode, static_cast with NDEBUG.
      template <typename To, typename From>
      inline
      To dyn_cast(From&& from)
      {
#ifdef NDEBUG
        return static_cast<To>(std::forward<From>(from));
#else
        return dynamic_cast<To>(std::forward<From>(from));
#endif
      }
    }

    // vcsn/dyn/automaton.hh.
    namespace detail
    {
      class automaton_base;
      template <typename Aut>
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
    using context = std::shared_ptr<const detail::context_base>;

    // vcsn/dyn/expansion.hh.
    namespace detail
    {
      class expansion_base;
      template <typename Aut>
      class expansion_wrapper;
    }
    using expansion = std::shared_ptr<const detail::expansion_base>;

    // vcsn/dyn/label.hh.
    namespace detail
    {
      class label_base;
      template <typename T>
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
      template <typename RatExpSet>
      class expression_wrapper;
    }
    using expression = std::shared_ptr<detail::expression_base>;

    // vcsn/dyn/expressionset.hh
    namespace detail
    {
      class expressionset_base;
      template <typename RatExpSet>
      class expressionset_wrapper;
    }
    using expressionset = std::shared_ptr<const detail::expressionset_base>;

    // vcsn/dyn/weight.hh.
    namespace detail
    {
      class weight_base;
      template <typename T>
      class weight_wrapper;
    }
    using weight = std::shared_ptr<const detail::weight_base>;
  }

  namespace ast
  {
    class ast_node;
    class automaton;
    class context;
    class genset;
    class letterset;
    class nullableset;
    class oneset;
    class other;
    class polynomialset;
    class expressionset;
    class tupleset;
    class weightset;
    class wordset;
  }
} // namespace vcsn

// FIXME: Not the best place for this.
#define REGISTER_DECLARE(Name, Signature)                       \
  using Name ## _t = auto Signature;                            \
  LIBVCSN_API                                                   \
  bool Name ## _register(const signature& sig, Name ## _t fn)
