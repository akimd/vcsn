#pragma once

#include <vector>

#include <boost/range.hpp> // make_iterator_range

#include <vcsn/core/rat/fwd.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace rat
  {

    /// The abstract, non-parameterized, root for all rational
    /// expression node types.
    class exp
    {
    public:
      virtual ~exp() = default;

      /// The possible types of expressions.
      using type_t = rat::type_t;

      /// The type of this node.
      virtual type_t type() const = 0;

      /// Whether star, complement.
      bool is_unary() const
      {
        return vcsn::rat::is_unary(type());
      }

      /// Whether a leaf of the expression tree.
      bool is_leaf() const
      {
        type_t t = type();
        return (vcsn::rat::is_constant(t)
                || t == type_t::atom);
      }
    };


    /*-------.
    | node.  |
    `-------*/

    /// The abstract parameterized, root for all rational expression
    /// types.
    ///
    /// The rational expressions are values, and as such, they are
    /// "stupid", and just "contain" some value without knowing how to
    /// interpret them (very much the same way as "bool" weights do
    /// not know whether they below to F2 or to B).
    ///
    /// However, in order to support hash-consing on the expression,
    /// expressions need to be able to hash and equality-compare the
    /// expressions, which means to be able to hash the labels and
    /// weights, which is something that only labelset and weightset
    /// can do.
    ///
    /// So there are two options:
    ///
    /// a. have the expression know the labelset and weightset (their
    /// types suffices, we don't need a value as the hash function is
    /// static in valuesets), i.e., have the expression know its context
    /// type.
    ///
    /// b. move the hash and equality-compare functions from valueset
    /// to value.  This seems wrong, as it would prevent the valueset
    /// from using equivalences to between values, or use a specific
    /// order.
    ///
    /// So nodes are parameterized by Context, instead of only
    /// <Label, Weight> as was the case before.
    template <typename Context>
    class node
      : public std::enable_shared_from_this<node<Context>>
      , public exp
    {
    public:
      using context_t = Context;
      using node_t = rat::node<context_t>;
      /// An expression usable with value semantics.
      using value_t = std::shared_ptr<const node_t>;
      using values_t = std::vector<value_t>;
      using const_visitor = vcsn::rat::const_visitor<context_t>;

      virtual void accept(const_visitor& v) const = 0;
    };

    /*--------.
    | inner.  |
    `--------*/

    /// An inner node.
    template <typename Context>
    class inner
      : public node<Context>
    {
    public:
      using super_t = node<Context>;
      using value_t = typename super_t::value_t;
    };


    /*-----------.
    | variadic.  |
    `-----------*/

    /// An inner node with multiple children.
    ///
    /// Implements the Composite Design Pattern.
    template <exp::type_t Type, typename Context>
    class variadic
      : public inner<Context>
    {
    public:
      static_assert(vcsn::rat::is_variadic(Type), "invalid type");

      using super_t = inner<Context>;
      using value_t = typename super_t::value_t;
      using values_t = typename super_t::values_t;

      using const_iterator = typename values_t::const_iterator;
      // Needed by boost::make_iterator_range, but since we iterate
      // over const value (well, shared_ptr to const values), make it
      // a const_iterator anyway.  Otherwise, clang won't compile.
      using iterator = const_iterator;
      using const_reverse_iterator = typename values_t::const_reverse_iterator;
      using reverse_iterator = const_reverse_iterator;

      virtual type_t type() const { return Type; };

      const_iterator begin() const;
      const_iterator end() const;
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      size_t size() const;

      /// Access the n-th element.
      /// precondition 0 <= n < size.
      const value_t operator[](size_t n) const;

      /// The first item of this variadic.
      const value_t head() const;
      /// The last item of this variadic.
      const value_t back() const;

      /// The non-first items.
      auto tail() const -> decltype(boost::make_iterator_range(*this, 1, 0));

      variadic(values_t ns = values_t());

      template <typename... Vs>
      variadic(Vs&&... vs)
        : sub_{std::forward<Vs>(vs)...}
      {}

      variadic(const variadic& that)
        : super_t(that)
        , sub_(that.sub_)
      {}

      /// Return a copy of children.
      values_t subs() const;

      virtual void accept(typename super_t::const_visitor& v) const;

    private:
      values_t sub_;
    };

    /*---------.
    | tuple.   |
    `---------*/

    template <typename Context, bool Enable>
    class tuple
      : public inner<Context>
    {
    public:
      static_assert(Context::is_lat,
                    "tuple: requires a tupleset labelset");
      using super_t = inner<Context>;
      using context_t = Context;
      using value_t = typename super_t::value_t;

      /// Given a tape, its corresponding expression type.
      template <unsigned Tape>
      using value_t_of
        = std::shared_ptr<const rat::node<detail::project_context<Tape, context_t>>>;

      template <typename Sequence>
      struct values_t_impl;

      template <size_t... I>
      struct values_t_impl<detail::index_sequence<I...>>
      {
        using type = std::tuple<value_t_of<I>...>;
      };
      using indices_t = typename labelset_t_of<context_t>::indices_t;
      using values_t = typename values_t_impl<typename indices_t::type>::type;

      template <typename... Args>
      tuple(Args&&... args)
        : sub_{std::forward<Args>(args)...}
      {};
      virtual type_t type() const { return type_t::tuple; };

      virtual void accept(typename super_t::const_visitor& v) const
      {
        v.visit(*this);
      }

      const values_t sub() const { return sub_; }

    private:
      values_t sub_;
    };

    template <typename Context>
    class tuple<Context, false>
      : public inner<Context>
    {};

    /*--------.
    | unary.  |
    `--------*/

    template <exp::type_t Type, typename Context>
    class unary
      : public inner<Context>
    {
    public:
      static_assert(is_unary(Type), "invalid type");

      using super_t = inner<Context>;
      using value_t = typename super_t::value_t;

      unary(value_t exp);
      virtual type_t type() const { return Type; };

      const value_t sub() const;

      virtual void accept(typename super_t::const_visitor& v) const;

    private:
      value_t sub_;
    };


    /*--------.
    | weight. |
    `--------*/

    /// An inner node implementing a weight.
    ///
    /// Implements the Composite Design Pattern.
    template <exp::type_t Type, typename Context>
    class weight_node
      : public inner<Context>
    {
    public:
      static_assert(Type == type_t::lweight
                    || Type == type_t::rweight,
                    "invalid type");

      using super_t = inner<Context>;
      using weight_t = weight_t_of<Context>;
      using value_t = typename super_t::value_t;

      virtual type_t type() const { return Type; };

      const value_t sub() const;
      const weight_t& weight() const;
      void set_weight(weight_t w);

      weight_node(weight_t w, value_t exp);
      weight_node(const weight_node& that)
        : sub_(that.sub_)
        , weight_(that.weight_)
      {}

      virtual void accept(typename super_t::const_visitor& v) const;

    private:
      value_t sub_;
      weight_t weight_;
    };


    /*------.
    | name. |
    `------*/

    /// An inner node to name the subexpression.
    template <typename Context>
    class name
      : public inner<Context>
    {
    public:
      using super_t = inner<Context>;
      using value_t = typename super_t::value_t;

      name(value_t exp, symbol name);

      const value_t sub() const;
      symbol name_get() const;

      virtual type_t type() const { return type_t::name; };
      virtual void accept(typename super_t::const_visitor& v) const;

    private:
      value_t sub_;
      symbol name_;
    };


    /*-------.
    | leaf.  |
    `-------*/

    /// The root from which to derive the final node types.
    template <typename Context>
    class leaf
      : public node<Context>
    {
    public:
      using super_t = node<Context>;
    };


    template <exp::type_t Type, typename Context>
    class constant
      : public leaf<Context>
    {
    public:
      static_assert(is_constant(Type), "invalid type");
      using super_t = leaf<Context>;
      using value_t = typename super_t::value_t;
      using type_t = typename super_t::type_t;

      virtual type_t type() const { return Type; };

      virtual void accept(typename super_t::const_visitor& v) const;
    };


    template <typename Context>
    class atom
      : public leaf<Context>
    {
    public:
      using super_t = leaf<Context>;
      using label_t = label_t_of<Context>;
      using type_t = typename super_t::type_t;
      using value_t = typename super_t::value_t;

      atom(const label_t& value);

      virtual type_t type() const { return type_t::atom; };

      virtual void accept(typename super_t::const_visitor& v) const;
      const label_t& value() const;

    private:
      label_t value_;
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/expression.hxx>
