#ifndef VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_
# define VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_

# include <iostream>
# include "visitor.hh"

namespace vcsn {
  namespace rat_exp {

    class PrintVisitor : public visitor
    {
    public:
      PrintVisitor(std::ostream &out);
      virtual ~PrintVisitor();
    public:
      virtual void visit(vcsn::rat_exp::exp &           v);
      virtual void visit(vcsn::rat_exp::concat &        v);
      virtual void visit(vcsn::rat_exp::plus &          v);
      virtual void visit(vcsn::rat_exp::kleene &        v);
      virtual void visit(vcsn::rat_exp::one &           v);
      virtual void visit(vcsn::rat_exp::zero &          v);
      virtual void visit(vcsn::rat_exp::word &          v);
      virtual void visit(vcsn::rat_exp::left_weight &   v);
      virtual void visit(vcsn::rat_exp::right_weight &  v);
    private:
      std::ostream &out_;
    };

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_
