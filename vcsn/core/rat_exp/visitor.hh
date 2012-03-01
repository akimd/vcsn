#ifndef VCSN_CORE_RAT_EXP_VISITOR_HH_
# define VCSN_CORE_RAT_EXP_VISITOR_HH_

# include <misc/static.hh>
# include <core/rat_exp/node.fwd.hh>

namespace vcsn {
  namespace rat_exp {

    class visitor
    {
    public:
      visitor();
      virtual ~visitor();
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
    };

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HH_
