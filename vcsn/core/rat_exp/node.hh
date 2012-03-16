#ifndef VCSN_RAT_EXP_NODE_HH_
# define VCSN_RAT_EXP_NODE_HH_

# include <list>
# include <string>

# include <core/rat_exp/node.fwd.hh>
# include <core/rat_exp/visitor.hh>

namespace vcsn
{
  namespace rat_exp
  {

    class RatExp
    {
    public:
      virtual ~RatExp() = 0;
    };

    template <class WeightSet>
    class RatExpNode : public RatExp
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      enum DynamicType
        {
          CONCAT,
          PLUS,
          KLEENE,
          ONE,
          ZERO,
          WORD,
        };
      enum WeightType
        {
          L_WEIGHT,
          LR_WEIGHT
        };
    protected:
      typedef typename visitor_traits<WeightSet>::Visitor Visitor;
      typedef typename visitor_traits<WeightSet>::ConstVisitor ConstVisitor;

    public:
      RatExpNode();
      virtual ~RatExpNode() = 0;
    public:
      virtual void accept(Visitor &v) = 0;
      virtual void accept(ConstVisitor &v) const = 0;
    public:
      virtual DynamicType type() const = 0;
      virtual WeightType weight_type() const = 0;
    };

    template <class WeightSet>
    class LRWeightNode : public RatExpNode<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef RatExpNode<WeightSet> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      LRWeightNode();
      LRWeightNode(const weightset_t& ws);
    public:
      virtual ~LRWeightNode() = 0;
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
    const typename LRWeightNode<WeightSet>::weightset_t
    LRWeightNode<WeightSet>::st_ws_ = WeightSet();

    template <class WeightSet>
    class LWeightNode : public RatExpNode<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef RatExpNode<WeightSet> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      LWeightNode();
      LWeightNode(const weightset_t& ws);
    public:
      virtual ~LWeightNode() = 0;
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
    const typename LWeightNode<WeightSet>::weightset_t
    LWeightNode<WeightSet>::st_ws_ = WeightSet();

    template <class WeightSet>
    class RatExpConcat : public LRWeightNode<WeightSet>
    {
    public:
      typedef LRWeightNode<WeightSet> super_type;
      typedef RatExpNode<WeightSet> root_type;
      typedef typename root_type::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::list<RatExpNode<WeightSet> *> node_list;

      typedef typename node_list::const_iterator         const_iterator;
      typedef typename node_list::iterator               iterator;
      typedef typename node_list::const_reverse_iterator const_reverse_iterator;
      typedef typename node_list::reverse_iterator       reverse_iterator;
    public:
      RatExpConcat();
      RatExpConcat(weightset_t& ws);
      virtual ~RatExpConcat();
    public:
      virtual DynamicType type() const { return root_type::CONCAT; };

      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      RatExpConcat<WeightSet>& push_back(RatExpNode<WeightSet>* elt);
      RatExpConcat<WeightSet>& push_front(RatExpNode<WeightSet>* elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor& v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor& v) const;

    private:
      node_list sub_node_;
    };

    template <class WeightSet>
    class RatExpPlus : public LRWeightNode<WeightSet>
    {
    public:
      typedef LRWeightNode<WeightSet> super_type;
      typedef RatExpNode<WeightSet> root_type;
      typedef typename root_type::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::list<RatExpNode<WeightSet> *> node_list;

      typedef typename node_list::const_iterator         const_iterator;
      typedef typename node_list::iterator               iterator;
      typedef typename node_list::const_reverse_iterator const_reverse_iterator;
      typedef typename node_list::reverse_iterator       reverse_iterator;
    public:
      RatExpPlus();
      RatExpPlus(const weightset_t& ws);
      virtual ~RatExpPlus();
    public:
      virtual DynamicType type() const { return root_type::PLUS; };

      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      RatExpPlus<WeightSet>& push_back(RatExpNode<WeightSet>* elt);
      RatExpPlus<WeightSet>& push_front(RatExpNode<WeightSet>* elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor& v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor& v) const;

    private:
      node_list sub_node_;
    };

    template <class WeightSet>
    class RatExpKleene : public LRWeightNode<WeightSet>
    {
    public:
      typedef LRWeightNode<WeightSet> super_type;
      typedef RatExpNode<WeightSet> root_type;
      typedef typename root_type::DynamicType DynamicType;

      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      RatExpKleene(RatExpNode<WeightSet>* sub_exp);
      RatExpKleene(RatExpNode<WeightSet>* sub_exp, const weightset_t& ws);
      virtual ~RatExpKleene();
    public:
      RatExpNode<WeightSet> *get_sub();
      const RatExpNode<WeightSet> *get_sub() const;
    public:
      virtual DynamicType type() const { return root_type::KLEENE; };

      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;

    private:
      RatExpNode<WeightSet> *sub_exp_;
    };

    template <class WeightSet>
    class RatExpOne : public LWeightNode<WeightSet>
    {
    public:
      typedef LWeightNode<WeightSet> super_type;
      typedef RatExpNode<WeightSet> root_type;
      typedef typename root_type::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      RatExpOne();
      virtual ~RatExpOne();
    public:
      virtual DynamicType type() const { return root_type::ONE; };

      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;
    };

    template <class WeightSet>
    class RatExpZero : public LWeightNode<WeightSet>
    {
    public:
      typedef LWeightNode<WeightSet> super_type;
      typedef RatExpNode<WeightSet> root_type;
      typedef typename root_type::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      RatExpZero();
      virtual ~RatExpZero();
    public:
      virtual DynamicType type() const { return root_type::ZERO; };

      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;
    };

    template <class WeightSet>
    class RatExpWord : public LWeightNode<WeightSet>
    {
    public:
      typedef LWeightNode<WeightSet> super_type;
      typedef RatExpNode<WeightSet> root_type;
      typedef typename root_type::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      RatExpWord(std::string* word);
      RatExpWord(std::string* word, const weightset_t& ws);
      virtual ~RatExpWord();
    public:
      virtual DynamicType type() const { return root_type::WORD; };

      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;
      std::string *get_word();
      const std::string *get_word() const;
    private:
      std::string *word_;
    };

  } // rat_exp
} // vcsn

#include <core/rat_exp/node.hxx>

#endif // !VCSN_RAT_EXP_NODE_HH_
