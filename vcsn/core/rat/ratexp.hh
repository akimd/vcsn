#ifndef VCSN_CORE_RAT_RATEXP_HH
# define VCSN_CORE_RAT_RATEXP_HH

# include <vector>
# include <string>

# include <boost/range.hpp> // make_iterator_range

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    /// The abstract, non-parameterized, root for all rational
    /// expression node types.
    class exp
    {
    public:
      virtual ~exp() = 0;

      /// The possible types of ratexps.
      using type_t = rat::type_t;

      /// The type of this node.
      virtual type_t type() const = 0;

      /// Whether star, complement.
      bool is_unary() const
      {
        return vcsn::rat::is_unary(type());
      }

      /// Whether one of the variadic types.
      bool is_nary() const
      {
        return vcsn::rat::is_nary(type());
      }

      /// Whether sum, prod, intersection, shuffle, star.
      bool is_inner() const
      {
        type_t t = type();
        return (vcsn::rat::is_unary(t)
		|| vcsn::rat::is_nary(t)
		|| t == type_t::lweight
		|| t == type_t::rweight);
      }
    };


    /*-------.
    | node.  |
    `-------*/

    /// The abstract parameterized, root for all rational expression
    /// types.
    template <typename Label, typename Weight>
    class node : public std::enable_shared_from_this<node<Label, Weight>>
               , public exp
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using node_t = rat::node<label_t, weight_t>;
      using self_t = node;
      using value_t = std::shared_ptr<const node_t>;
      /// Same as value_t, but writable.  Use with care.
      using wvalue_t = std::shared_ptr<node_t>;
      using ratexps_t = std::vector<value_t>;
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      using shared_t = std::shared_ptr<const node>;
      shared_t clone() const;

      virtual void accept(const_visitor &v) const = 0;

    protected:
      virtual value_t clone_() const = 0;
    };

    /*--------.
    | inner.  |
    `--------*/

    /// An inner node.
    template <typename Label, typename Weight>
    class inner : public node<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = node<label_t, weight_t>;
      using value_t = typename super_type::value_t;
      using self_t = inner;

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;

    protected:
      virtual value_t clone_() const = 0;
    };


    /*-------.
    | nary.  |
    `-------*/

    /// An inner node with multiple children.
    ///
    /// Implements the Composite Design Pattern.
    template <exp::type_t Type, typename Label, typename Weight>
    class nary: public inner<Label, Weight>
    {
    public:
      static_assert(vcsn::rat::is_nary(Type), "invalid type");

      using label_t = Label;
      using weight_t = Weight;
      using super_type = inner<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using value_t = typename super_type::value_t;
      using ratexps_t = typename super_type::ratexps_t;
      using self_t = nary;

      using const_iterator = typename ratexps_t::const_iterator;
      // Needed by boost::make_iterator_range, but since we iterate
      // over const value (well, shared_ptr to const values), make it
      // a const_iterator anyway.  Otherwise, clang won't compile.
      using iterator = const_iterator;
      using const_reverse_iterator = typename ratexps_t::const_reverse_iterator;
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

      /// The first item of this nary.
      const value_t head() const;

      /// The non-first items.
      auto tail() const -> decltype(boost::make_iterator_range(*this, 1, 0));

      nary(const ratexps_t& ns = ratexps_t());
      nary(const nary& that)
        : super_type(that)
        , sub_ratexp_(that.sub_ratexp_)
      {}

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;

      virtual void accept(typename node_t::const_visitor &v) const;

    private:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }

      ratexps_t sub_ratexp_;
    };

    /*--------.
    | unary.  |
    `--------*/

    template <exp::type_t Type, typename Label, typename Weight>
    class unary : public inner<Label, Weight>
    {
    public:
      static_assert(is_unary(Type), "invalid type");

      using label_t = Label;
      using weight_t = Weight;
      using super_type = inner<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using value_t = typename node_t::value_t;
      using self_t = unary;

      unary(value_t exp);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;

      virtual type_t type() const { return Type; };

      const value_t sub() const;

      virtual void accept(typename node_t::const_visitor &v) const;

    private:
      value_t sub_exp_;

      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    /*--------.
    | weight. |
    `--------*/

    /// An inner node implementing a weight.
    ///
    /// Implements the Composite Design Pattern.
    template <exp::type_t Type, typename Label, typename Weight>
    class weight_node: public inner<Label, Weight>
    {
    public:
      static_assert(Type == type_t::lweight
                    || Type == type_t::rweight,
                    "invalid type");

      using label_t = Label;
      using weight_t = Weight;
      using super_type = inner<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using value_t = typename super_type::value_t;
      using self_t = weight_node;

      virtual type_t type() const { return Type; };

      const value_t sub() const;
      const weight_t& weight() const;
      void set_weight(const weight_t& w);

      weight_node(const weight_t& w, value_t exp);
      weight_node(const weight_node& that)
        : sub_exp_(that.sub_exp_)
        , weight_(that.weight_)
      {}

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;

      virtual void accept(typename node_t::const_visitor &v) const;

    private:
      value_t sub_exp_;
      weight_t weight_;

      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    /*-------.
    | leaf.  |
    `-------*/

    /// The root from which to derive the final node types.
    template <typename Label, typename Weight>
    class leaf : public node<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using super_type = node_t;
      using self_t = leaf;
    protected:
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;
      virtual value_t clone_() const = 0;
    };


    template <exp::type_t Type, typename Label, typename Weight>
    class constant : public leaf<Label, Weight>
    {
    public:
      static_assert(Type == type_t::zero
                    || Type == type_t::one,
                    "invalid type");
      using label_t = Label;
      using weight_t = Weight;
      using super_type = leaf<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using value_t = typename node_t::value_t;
      using self_t = constant;

      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;

      virtual type_t type() const { return Type; };

      virtual void accept(typename node_t::const_visitor &v) const;
    protected:
      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };


    template <typename Label, typename Weight>
    class atom : public leaf<Label, Weight>
    {
    public:
      using label_t = Label;
      using weight_t = Weight;
      using super_type = leaf<label_t, weight_t>;
      using node_t = node<label_t, weight_t>;
      using type_t = typename node_t::type_t;
      using value_t = typename node_t::value_t;
      using self_t = atom;

      atom(const label_t& value);
      using shared_t = std::shared_ptr<const self_t>;
      shared_t clone() const;

      virtual type_t type() const { return type_t::atom; };

      virtual void accept(typename node_t::const_visitor &v) const;
      const label_t& value() const;

    private:
      label_t value_;

      virtual value_t clone_() const
      {
        return std::make_shared<self_t>(*this);
      }
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/ratexp.hxx>

#endif // !VCSN_CORE_RAT_RATEXP_HH
