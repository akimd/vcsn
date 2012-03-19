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

    template <class WeightSet>
    class node : public exp
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    protected:
      typedef typename visitor_traits<WeightSet>::visitor visitor;
      typedef typename visitor_traits<WeightSet>::const_visitor const_visitor;

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

    template <class WeightSet>
    class weighted : public node<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef node<WeightSet> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      weighted();
      weighted(const weightset_t& ws);
    public:
      virtual ~weighted() = 0;
    public:
      const weightset_t &get_weight_set() const;
      const weight_t &left_weight() const;
      weight_t &left_weight();

      const weight_t &right_weight() const;
      weight_t &right_weight();

      virtual WeightType weight_type() const;
    protected:
      const static weightset_t st_ws_;
      const weightset_t& ws_;
      weight_t lw_;
      weight_t rw_;
    };

    template<class WeightSet>
    const typename weighted<WeightSet>::weightset_t
    weighted<WeightSet>::st_ws_ = WeightSet();

    template <class WeightSet>
    class left_weighted : public node<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef node<WeightSet> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      left_weighted();
      left_weighted(const weightset_t& ws);
    public:
      virtual ~left_weighted() = 0;
    public:
      const weightset_t &get_weight_set() const;
      const weight_t &left_weight() const;
      weight_t &left_weight();

      virtual WeightType weight_type() const;
    protected:
      const static weightset_t st_ws_;
      const weightset_t& ws_;
      weight_t lw_;
    };

    template <class WeightSet>
    const typename left_weighted<WeightSet>::weightset_t
    left_weighted<WeightSet>::st_ws_ = WeightSet();

    template <class WeightSet>
    class concat : public weighted<WeightSet>
    {
    public:
      typedef weighted<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::type_t type_t;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
    public:
      concat();
      concat(weightset_t& ws);
      virtual ~concat();
    public:
      virtual type_t type() const { return node_t::CONCAT; };

      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      concat<WeightSet>& push_back(node_t* elt);
      concat<WeightSet>& push_front(node_t* elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename node_t::visitor& v);
      virtual void accept(typename node_t::const_visitor& v) const;

    private:
      nodes_t sub_node_;
    };

    template <class WeightSet>
    class plus : public weighted<WeightSet>
    {
    public:
      typedef weighted<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::type_t type_t;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
    public:
      plus();
      plus(const weightset_t& ws);
      virtual ~plus();
    public:
      virtual type_t type() const { return node_t::PLUS; };

      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      plus<WeightSet>& push_back(node_t* elt);
      plus<WeightSet>& push_front(node_t* elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename node_t::visitor& v);
      virtual void accept(typename node_t::const_visitor& v) const;

    private:
      nodes_t sub_node_;
    };

    template <class WeightSet>
    class kleene : public weighted<WeightSet>
    {
    public:
      typedef weighted<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::type_t type_t;

      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      kleene(node_t* sub_exp);
      kleene(node_t* sub_exp, const weightset_t& ws);
      virtual ~kleene();
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

    template <class WeightSet>
    class one : public left_weighted<WeightSet>
    {
    public:
      typedef left_weighted<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::type_t type_t;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      one();
      virtual ~one();
    public:
      virtual type_t type() const { return node_t::ONE; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
    };

    template <class WeightSet>
    class zero : public left_weighted<WeightSet>
    {
    public:
      typedef left_weighted<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::type_t type_t;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      zero();
      virtual ~zero();
    public:
      virtual type_t type() const { return node_t::ZERO; };

      virtual void accept(typename node_t::visitor &v);
      virtual void accept(typename node_t::const_visitor &v) const;
    };

    template <class WeightSet>
    class word : public left_weighted<WeightSet>
    {
    public:
      typedef left_weighted<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::type_t type_t;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      word(std::string* word);
      word(std::string* word, const weightset_t& ws);
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

  } // rat
} // vcsn

#include <core/rat/node.hxx>

#endif // !VCSN_RAT_NODE_HH_
