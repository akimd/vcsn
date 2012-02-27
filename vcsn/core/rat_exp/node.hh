#ifndef VCSN_RAT_EXP_NODE_HH_
# define VCSN_RAT_EXP_NODE_HH_

# include <list>
# include <string>

# include "node.fwd.hh"
# include "visitor.fwd.hh"

namespace vcsn {
  namespace rat_exp {

    class exp
    {
    public:
      virtual void accept(vcsn::rat_exp::visitor &v) = 0;
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

      virtual void accept(vcsn::rat_exp::visitor &v);

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

      virtual void accept(vcsn::rat_exp::visitor &v);

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
      virtual void accept(vcsn::rat_exp::visitor &v);
      exp *get_sub();

    private:
      exp *sub_exp_;
    };

    class one : public exp
    {
    public:
      one();
      ~one();
    public:
      virtual void accept(vcsn::rat_exp::visitor &v);
    };

    class zero : public exp
    {
    public:
      zero();
      ~zero();
    public:
      virtual void accept(vcsn::rat_exp::visitor &v);
    };

    class word : public exp
    {
    public:
      word(std::string *word);
      ~word();
    public:
      virtual void accept(vcsn::rat_exp::visitor &v);
      std::string *get_word();

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
      virtual void accept(vcsn::rat_exp::visitor &v);
      exp *get_exp();
      weight *get_weight();

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
      virtual void accept(vcsn::rat_exp::visitor &v);
      exp *get_exp();
      weight *get_weight();

    private:
      exp *l_exp_;
      weight *r_weight_;
    };

  } // !rat_exp
} // !vcsn

#include "node.hxx"

#endif // !VCSN_RAT_EXP_NODE_HH_
