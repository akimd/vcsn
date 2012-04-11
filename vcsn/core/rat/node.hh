#ifndef VCSN_CORE_RAT_NODE_HH
# define VCSN_CORE_RAT_NODE_HH

# include <list>
# include <string>

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

      enum type_t
        {
          ZERO = 0,
          ONE  = 1,
          ATOM = 2,
          SUM  = 3,
          PROD = 4,
          STAR = 5,
        };
      virtual type_t type() const = 0;
    };


    /*-------.
    | node.  |
    `-------*/

    template <class Weight>
    class node : public exp
    {
    public:
      using weight_t = Weight;
    protected:
      using visitor = typename visitor_traits<Weight>::visitor;
      using const_visitor = typename visitor_traits<Weight>::const_visitor;

    public:
      node(const weight_t& l);
      virtual bool is_leaf() const = 0;

    public:
      virtual void accept(visitor &v) = 0;
      virtual void accept(const_visitor &v) const = 0;

    public:
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
    protected:
      inner(const weight_t& l, const weight_t& r);
    public:
      virtual bool is_leaf() const { return false; };
    public:
      const weight_t &right_weight() const;
      weight_t &right_weight();

    protected:
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
      using nodes_t = std::list<node_t*>;

      using const_iterator = typename nodes_t::const_iterator        ;
      using iterator = typename nodes_t::iterator              ;
      using const_reverse_iterator = typename nodes_t::const_reverse_iterator;
      using reverse_iterator = typename nodes_t::reverse_iterator      ;
    public:
      nary(const weight_t& l, const weight_t& r);
      virtual ~nary();
    public:
      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      nary& push_back(node_t* elt);
      nary& push_front(node_t* elt);
      size_t size() const;
      void erase(iterator it);
      void erase(iterator begin, iterator end);
      void clear();
      void splice(iterator it, nary& right);

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
      using nodes_t = std::list<node_t*>;

      using const_iterator = typename nodes_t::const_iterator        ;
      using iterator = typename nodes_t::iterator              ;
      using const_reverse_iterator = typename nodes_t::const_reverse_iterator;
      using reverse_iterator = typename nodes_t::reverse_iterator      ;
    public:
      prod(const weight_t& l, const weight_t& r);
    public:
      virtual type_t type() const { return node_t::PROD; };

    public:
      virtual void accept(typename node_t::visitor& v);
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
      using nodes_t = std::list<node_t*>;

      using const_iterator = typename nodes_t::const_iterator        ;
      using iterator = typename nodes_t::iterator              ;
      using const_reverse_iterator = typename nodes_t::const_reverse_iterator;
      using reverse_iterator = typename nodes_t::reverse_iterator      ;
    public:
      sum(const weight_t& l, const weight_t& r);

    public:
      virtual type_t type() const { return node_t::SUM; };

    public:
      virtual void accept(typename node_t::visitor& v);
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

    public:
      star(const weight_t& l, const weight_t& r, node_t* exp);
      virtual ~star();
    public:
      node_t *get_sub();
      const node_t *get_sub() const;
    public:
      virtual type_t type() const { return node_t::STAR; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;

    private:
      node_t *sub_exp_;
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
    public:
      virtual bool is_leaf() const { return true; };
    };


    template <class Weight>
    class one : public leaf<Weight>
    {
    public:
      using weight_t = Weight;
      using super_type = leaf<weight_t>;
      using node_t = node<weight_t>;
      using type_t = typename node_t::type_t;
    public:
      one(const weight_t& l);
    public:
      virtual type_t type() const { return node_t::ONE; };

      virtual void accept(typename node_t::visitor &v);
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
    public:
      zero(const weight_t& l);
    public:
      virtual type_t type() const { return node_t::ZERO; };

      virtual void accept(typename node_t::visitor &v);
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
    public:
      atom(const weight_t& l, const std::string& atom);
    public:
      virtual type_t type() const { return node_t::ATOM; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
      const std::string& get_atom() const;
    private:
      std::string atom_;
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/node.hxx>

#endif // !VCSN_CORE_RAT_NODE_HH
