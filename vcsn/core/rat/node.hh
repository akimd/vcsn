#ifndef VCSN_CORE_RAT_NODE_HH
# define VCSN_CORE_RAT_NODE_HH

# include <list>
# include <string>

# include <boost/range.hpp>

# include <vcsn/core/rat/node.fwd.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    class exp
    {
    public:
      virtual ~exp() = 0;

      /// The possible types of nodes.
      enum type_t
        {
          ZERO = 0,
          ONE  = 1,
          ATOM = 2,
          SUM  = 3,
          PROD = 4,
          STAR = 5,
        };

      /// The type of this node.
      virtual type_t type() const = 0;

      /// Whether sum, prod, or star.
      bool is_inner() const
      {
        type_t t = type();
        return t == SUM || t == PROD || t == STAR;
      }
    };


    /*-------.
    | node.  |
    `-------*/

    template <class Weight>
    class node : public exp
    {
    public:
      using weight_t = Weight;
      using node_t = rat::node<weight_t>;
      using kvalue_t = std::shared_ptr<node_t>;
      using nodes_t = std::list<kvalue_t>;
      using const_visitor = vcsn::rat::const_visitor<weight_t>;

      node(const weight_t& l);
      node(const node& that)
        : lw_(that.lw_)
      {}

      virtual void accept(const_visitor &v) const = 0;

      const weight_t &left_weight() const;
      weight_t &left_weight();

    protected:
      weight_t lw_;
    };

    /*--------.
    | inner.  |
    `--------*/

    template <class Weight>
    class inner : public node<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = node<weight_t>;
      using kvalue_t = typename super_type::kvalue_t;

      const weight_t &right_weight() const;
      weight_t &right_weight();

    protected:
      inner(const weight_t& l, const weight_t& r);
      inner(const inner& that)
        : super_type(that)
        , rw_(that.rw_)
      {}
      weight_t rw_;
    };


    /*-------.
    | nary.  |
    `-------*/

    template <class Weight>
    class nary: public inner<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = inner<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;
      using kvalue_t = typename super_type::kvalue_t;
      using nodes_t = typename super_type::nodes_t;

      using const_iterator = typename nodes_t::const_iterator;
      using iterator = typename nodes_t::iterator;
      using const_reverse_iterator = typename nodes_t::const_reverse_iterator;
      using reverse_iterator = typename nodes_t::reverse_iterator;

      const_iterator begin() const;
      const_iterator end() const;
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      size_t size() const;

      /// The first item of this nary.
      const kvalue_t head() const { return *begin(); }

      /// The non-first items.
      auto tail() const -> decltype(boost::make_iterator_range(*this, 1, 0))
      {
        return boost::make_iterator_range(*this, 1, 0);
      }

    protected:
      nary(const weight_t& l, const weight_t& r, const nodes_t& ns = nodes_t());
      nary(const nary& that)
        : super_type(that)
        , sub_node_(that.sub_node_)
      {}

    private:
      nodes_t sub_node_;
    };

    /*-------.
    | prod.  |
    `-------*/

    template <class Weight>
    class prod : public nary<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = nary<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;
      using kvalue_t = typename node_t::kvalue_t;
      using nodes_t = typename node_t::nodes_t;

      using const_iterator = typename nodes_t::const_iterator;
      using iterator = typename nodes_t::iterator;
      using const_reverse_iterator = typename nodes_t::const_reverse_iterator;
      using reverse_iterator = typename nodes_t::reverse_iterator;

      prod(const weight_t& l, const weight_t& r, const nodes_t& ns = nodes_t());

      virtual type_t type() const { return node_t::PROD; };

      virtual void accept(typename node_t::const_visitor& v) const;
    };

    /*------.
    | sum.  |
    `------*/

    template <class Weight>
    class sum : public nary<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = nary<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;
      using kvalue_t = typename node_t::kvalue_t;
      using nodes_t = std::list<kvalue_t>;

      using const_iterator = typename nodes_t::const_iterator;
      using iterator = typename nodes_t::iterator;
      using const_reverse_iterator = typename nodes_t::const_reverse_iterator;
      using reverse_iterator = typename nodes_t::reverse_iterator;

      sum(const weight_t& l, const weight_t& r, const nodes_t& ns = nodes_t());

      virtual type_t type() const { return node_t::SUM; };

      virtual void accept(typename node_t::const_visitor& v) const;
    };

    /*-------.
    | star.  |
    `-------*/

    template <class Weight>
    class star : public inner<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = inner<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;
      using kvalue_t = typename node_t::kvalue_t;

      star(const weight_t& l, const weight_t& r, kvalue_t exp);

      virtual type_t type() const { return node_t::STAR; };

      kvalue_t get_sub();
      const kvalue_t get_sub() const;

      virtual void accept(typename node_t::const_visitor &v) const;

    private:
      kvalue_t sub_exp_;
    };


    /*-------.
    | leaf.  |
    `-------*/

    template <class Weight>
    class leaf : public node<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = node<weight_t>;
    protected:
      leaf(const weight_t& l);
    };


    template <class Weight>
    class one : public leaf<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = leaf<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;

      one(const weight_t& l);

      virtual type_t type() const { return node_t::ONE; };

      virtual void accept(typename node_t::const_visitor &v) const;
    };

    template <class Weight>
    class zero : public leaf<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = leaf<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;

      zero(const weight_t& l);

      virtual type_t type() const { return node_t::ZERO; };

      virtual void accept(typename node_t::const_visitor &v) const;
    };

    template <class Weight>
    class atom : public leaf<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = leaf<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;

      atom(const weight_t& l, const std::string& atom);

      virtual type_t type() const { return node_t::ATOM; };

      virtual void accept(typename node_t::const_visitor &v) const;
      const std::string& get_atom() const;
    private:
      std::string atom_;
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/node.hxx>

#endif // !VCSN_CORE_RAT_NODE_HH
