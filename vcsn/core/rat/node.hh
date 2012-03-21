#ifndef VCSN_RAT_NODE_HH_
# define VCSN_RAT_NODE_HH_

# include <list>
# include <string>

# include <core/rat/node.fwd.hh>
# include <core/rat/visitor.hh>

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
      node();
      virtual ~node() = 0;
    public:
      virtual void accept(visitor &v) = 0;
      virtual void accept(const_visitor &v) const = 0;
    public:
      enum WeightType
        {
          L_WEIGHT,
          LR_WEIGHT
        };
      virtual WeightType weight_type() const = 0;
    };

    /*-----------.
    | weighted.  |
    `-----------*/

    template <class Weight>
    class weighted : public node<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef node<weight_t> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      weighted(const weight_t& l, const weight_t& r);
    public:
      virtual ~weighted() = 0;
    public:
      const weight_t &left_weight() const;
      weight_t &left_weight();

      const weight_t &right_weight() const;
      weight_t &right_weight();

      virtual WeightType weight_type() const;
    protected:
      weight_t lw_;
      weight_t rw_;
    };


    /*-------.
    | nary.  |
    `-------*/

    template <class Weight>
    class nary: public weighted<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef weighted<weight_t> super_type;
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
    class star : public weighted<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef weighted<weight_t> super_type;
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
    class left_weighted : public node<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef node<weight_t> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      left_weighted(const weight_t& l);
    public:
      virtual ~left_weighted() = 0;
    public:
      const weight_t &left_weight() const;
      weight_t &left_weight();

      virtual WeightType weight_type() const;
    protected:
      weight_t lw_;
    };


    template <class Weight>
    class one : public left_weighted<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef left_weighted<weight_t> super_type;
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
    class zero : public left_weighted<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef left_weighted<weight_t> super_type;
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
    class word : public left_weighted<Weight>
    {
    public:
      typedef Weight weight_t;
      typedef left_weighted<weight_t> super_type;
      typedef node<weight_t> node_t;
      typedef typename node_t::type_t type_t;
    public:
      word(const weight_t& l, std::string* word);
      virtual ~word();
    public:
      virtual type_t type() const { return node_t::WORD; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
      std::string *get_word();
      const std::string *get_word() const;
    private:
      std::string *word_;
    };

  } // namespace rat
} // namespace vcsn

#include <core/rat/node.hxx>

#endif // !VCSN_RAT_NODE_HH_
