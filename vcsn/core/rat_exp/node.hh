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

    template<class WeightSet>
    class RatExpNode : public RatExp
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      enum DynamicType {
        CONCAT,
        PLUS,
        KLEENE,
        ONE,
        ZERO,
        WORD,
        LEFT_WEIGHT,
        RIGHT_WEIGHT
      };
      enum WeightType {
        L_WEIGHT,
        LR_WEIGHT
      };
    protected:
      typedef typename visitor_traits<WeightSet>::Visitor Visitor;
      typedef typename visitor_traits<WeightSet>::ConstVisitor ConstVisitor;

    public:
      RatExpNode(DynamicType dyn_type, WeightType weight_type);
      virtual ~RatExpNode() = 0;
    public:
      virtual void accept(Visitor &v) = 0;
      virtual void accept(ConstVisitor &v) const = 0;
    public:
      DynamicType getType() const;
      WeightType getWeightType() const;
    protected:
      const DynamicType dyn_type_;
      const WeightType weight_type_;
    };

    template<class WeightSet>
    class LRWeightNode : public RatExpNode<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    protected:
      LRWeightNode(typename RatExpNode<WeightSet>::DynamicType dyn_type);
      virtual ~LRWeightNode() = 0;
    public:
      const weight_t &left_weight() const;
      weight_t &left_weight();

      const weight_t &right_weight() const;
      weight_t &right_weight();
    protected:
      weight_t lw_;
      weight_t rw_;
    };

    template<class WeightSet>
    class LWeightNode : public RatExpNode<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    protected:
      LWeightNode(typename RatExpNode<WeightSet>::DynamicType dyn_type);
      virtual ~LWeightNode() = 0;
    public:
      const weight_t &left_weight() const;
      weight_t &left_weight();
    protected:
      weight_t lw_;
    };

    template<class WeightSet>
    class RatExpConcat : public LRWeightNode<WeightSet>
    {
    public:
      typedef std::list<RatExpNode<WeightSet> *> node_list;

      typedef typename node_list::const_iterator         const_iterator;
      typedef typename node_list::iterator               iterator;
      typedef typename node_list::const_reverse_iterator const_reverse_iterator;
      typedef typename node_list::reverse_iterator       reverse_iterator;
    public:
      RatExpConcat();
      virtual ~RatExpConcat();
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
      RatExpConcat<WeightSet> &push_back(RatExpNode<WeightSet> *elt);
      RatExpConcat<WeightSet> &push_front(RatExpNode<WeightSet> *elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;

    private:
      node_list sub_node_;
    };

    template<class WeightSet>
    class RatExpPlus : public LRWeightNode<WeightSet>
    {
    public:
      typedef std::list<RatExpNode<WeightSet> *> node_list;

      typedef typename node_list::const_iterator         const_iterator;
      typedef typename node_list::iterator               iterator;
      typedef typename node_list::const_reverse_iterator const_reverse_iterator;
      typedef typename node_list::reverse_iterator       reverse_iterator;
    public:
      RatExpPlus();
      virtual ~RatExpPlus();
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
      RatExpPlus<WeightSet> &push_back(RatExpNode<WeightSet> *elt);
      RatExpPlus<WeightSet> &push_front(RatExpNode<WeightSet> *elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;

    private:
      node_list sub_node_;
    };

    template<class WeightSet>
    class RatExpKleene : public LRWeightNode<WeightSet>
    {
    public:
      RatExpKleene(RatExpNode<WeightSet> *sub_exp);
      virtual ~RatExpKleene();
    public:
      RatExpNode<WeightSet> *getSubNode();
      const RatExpNode<WeightSet> *getSubNode() const;
    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;

    private:
      RatExpNode<WeightSet> *sub_exp_;
    };

    template<class WeightSet>
    class RatExpOne : public LWeightNode<WeightSet>
    {
    public:
      RatExpOne();
      virtual ~RatExpOne();
    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;
    };

    template<class WeightSet>
    class RatExpZero : public LWeightNode<WeightSet>
    {
    public:
      RatExpZero();
      virtual ~RatExpZero();
    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;
    };

    template<class WeightSet>
    class RatExpWord : public LWeightNode<WeightSet>
    {
    public:
      RatExpWord(std::string *word);
      virtual ~RatExpWord();
    public:
      virtual void accept(typename RatExpNode<WeightSet>::Visitor &v);
      virtual void accept(typename RatExpNode<WeightSet>::ConstVisitor &v) const;
      std::string *get_word();
      const std::string *get_word() const;
    private:
      std::string *word_;
    };

  } // !rat_exp
} // !vcsn

#include <core/rat_exp/node.hxx>

#endif // !VCSN_RAT_EXP_NODE_HH_
