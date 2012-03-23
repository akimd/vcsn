#ifndef VCSN_RAT_NODE_HH_
# define VCSN_RAT_NODE_HH_

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
          CONCAT,
          PLUS,
          KLEENE,
          ONE,
          ZERO,
          WORD,
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
      typedef Weight weight_t;
    protected:
      typedef typename visitor_traits<Weight>::visitor visitor;
      typedef typename visitor_traits<Weight>::const_visitor const_visitor;

    public:
      node(const weight_t& l);
      virtual ~node() = 0;
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
      typedef Weight weight_t;
      typedef node<weight_t> super_type;
    protected:
      inner(const weight_t& l, const weight_t& r);
    public:
      virtual ~inner() = 0;
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
      typedef Weight weight_t;
      typedef inner<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
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
      typedef Weight weight_t;
      typedef nary<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
    public:
      prod(const weight_t& l, const weight_t& r);
    public:
      virtual type_t type() const { return node_t::CONCAT; };

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
      typedef Weight weight_t;
      typedef nary<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
    public:
      sum(const weight_t& l, const weight_t& r);

    public:
      virtual type_t type() const { return node_t::PLUS; };

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
      typedef Weight weight_t;
      typedef inner<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;

    public:
      star(const weight_t& l, const weight_t& r, node_t* exp);
      virtual ~star();
    public:
      node_t *get_sub();
      const node_t *get_sub() const;
    public:
      virtual type_t type() const { return node_t::KLEENE; };

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
      typedef Weight weight_t;
      typedef node<weight_t> super_type;
    protected:
      leaf(const weight_t& l);
    public:
      virtual ~leaf() = 0;
      virtual bool is_leaf() const { return true; };
    };


    template <class Weight>
    class one : public leaf<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef leaf<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
    public:
      one(const weight_t& l);
      virtual ~one();
    public:
      virtual type_t type() const { return node_t::ONE; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
    };

    template <class Weight>
    class zero : public leaf<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef leaf<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
    public:
      zero(const weight_t& l);
      virtual ~zero();
    public:
      virtual type_t type() const { return node_t::ZERO; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
    };

    template <class Weight>
    class atom : public leaf<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef leaf<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
    public:
      atom(const weight_t& l, std::string* atom);
      virtual ~atom();
    public:
      virtual type_t type() const { return node_t::WORD; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
      std::string *get_atom();
      const std::string *get_atom() const;
    private:
      std::string *atom_;
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/node.hxx>

#endif // !VCSN_RAT_NODE_HH_
