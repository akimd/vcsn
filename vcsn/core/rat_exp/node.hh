#ifndef VCSN_RAT_EXP_NODE_HH_
# define VCSN_RAT_EXP_NODE_HH_

# include <list>
# include <string>

# include <core/rat_exp/node.fwd.hh>
# include <core/rat_exp/visitor.hh>

namespace vcsn {
  namespace rat_exp {

    class exp
    {
    public:
      virtual void accept(visitor &v) = 0;
      virtual void accept(ConstVisitor &v) const = 0;
    protected:
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
    protected:
      exp(DynamicType dyn_type);
    public:
      DynamicType getType() const;
    protected:
      const DynamicType dyn_type_;
    };

    class concat : public exp
    {
    public:
      typedef std::list<exp *> node_list;

      typedef node_list::const_iterator const_iterator;
      typedef node_list::iterator iterator;
      typedef node_list::const_reverse_iterator const_reverse_iterator;
      typedef node_list::reverse_iterator reverse_iterator;

    public:
      concat();
      ~concat();
    public:
      const_iterator begin() const;
      iterator begin();
      const_iterator end() const;
      iterator end();
      const_reverse_iterator rbegin() const;
      reverse_iterator rbegin();
      const_reverse_iterator rend() const;
      reverse_iterator rend();

    public:
      concat &push_front(exp *left_rat_exp);
      concat &push_back(exp *left_rat_exp);
      size_t size() const;

      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;

    private:
      node_list sub_node_;
    };

    class plus : public exp
    {
    public:
      typedef std::list<exp *> node_list;
      typedef node_list::const_iterator const_iterator;
      typedef node_list::iterator iterator;
      typedef node_list::const_reverse_iterator const_reverse_iterator;
      typedef node_list::reverse_iterator reverse_iterator;
    public:
      plus();
      ~plus();
    public:
      plus &push_front(exp *left_rat_exp);

      plus &push_back(exp *left_rat_exp);
      size_t size() const;

      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;

      const_iterator begin() const;
      iterator begin();
      const_iterator end() const;
      iterator end();
      const_reverse_iterator rbegin() const;
      reverse_iterator rbegin();
      const_reverse_iterator rend() const;
      reverse_iterator rend();
    private:
      node_list sub_node_;
    };

    class kleene : public exp
    {
    public:
      kleene(exp *sub_exp);
      ~kleene();
    public:
      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;
      exp *get_sub();
      const exp *get_sub() const;

    private:
      exp *sub_exp_;
    };

    class one : public exp
    {
    public:
      one();
      ~one();
    public:
      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;
    };

    class zero : public exp
    {
    public:
      zero();
      ~zero();
    public:
      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;
    };

    class word : public exp
    {
    public:
      word(std::string *word);
      ~word();
    public:
      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;
      std::string *get_word();
      const std::string *get_word() const;

    private:
      std::string *word_;
    };

    class left_weight : public exp
    {
    public:
      typedef weights_type weight;
    public:
      left_weight(weight *left_weight, exp *right_rat_exp);
      ~left_weight();
    public:
      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;
      exp *get_exp();
      weight *get_weight();

      const exp *get_exp() const;
      const weight *get_weight() const;

    private:
      weight *l_weight_;
      exp *r_exp_;
    };

    class right_weight : public exp
    {
    public:
      typedef weights_type weight;
    public:
      right_weight(exp *left_rat_exp, weight *right_weight);
      ~right_weight();
    public:
      virtual void accept(visitor &v);
      virtual void accept(ConstVisitor &v) const;
      exp *get_exp();
      weight *get_weight();

      const exp *get_exp() const;
      const weight *get_weight() const;

    private:
      exp *l_exp_;
      weight *r_weight_;
    };

  } // !rat_exp
} // !vcsn

#include <core/rat_exp/node.hxx>

#endif // !VCSN_RAT_EXP_NODE_HH_
