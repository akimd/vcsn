#ifndef VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_
# define VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_

# include <iostream>
# include <core/rat_exp/visitor.hh>

namespace vcsn {
  namespace rat_exp {

    class PrintVisitor : public visitor
    {
    public:
      PrintVisitor(std::ostream &out);
      virtual ~PrintVisitor();
    public:
      virtual void visit(exp &           v);
      virtual void visit(concat &        v);
      virtual void visit(plus &          v);
      virtual void visit(kleene &        v);
      virtual void visit(one &           v);
      virtual void visit(zero &          v);
      virtual void visit(word &          v);
      virtual void visit(left_weight &   v);
      virtual void visit(right_weight &  v);
    private:
      std::ostream &out_;
    };

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_
