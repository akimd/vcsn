#ifndef VCSN_CORE_RAT_EXP_VISITOR_HH_
# define VCSN_CORE_RAT_EXP_VISITOR_HH_

# include <misc/static.hh>
# include <core/rat_exp/node.fwd.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet, template <class> class ConstNess>
    class GenVisitor
    {
    public:
      typedef WeightSet weightset_t;
    public:
      virtual void visit(typename ConstNess<RatExpNode<WeightSet> >::type &          v);
      virtual void visit(typename ConstNess<RatExpConcat<WeightSet> >::type &        v);
      virtual void visit(typename ConstNess<RatExpPlus<WeightSet> >::type &          v);
      virtual void visit(typename ConstNess<RatExpKleene<WeightSet> >::type &        v);
      virtual void visit(typename ConstNess<RatExpOne<WeightSet> >::type &           v);
      virtual void visit(typename ConstNess<RatExpZero<WeightSet> >::type &          v);
      virtual void visit(typename ConstNess<RatExpWord<WeightSet> >::type &          v);
    };

    template <class WeightSet>
    struct visitor_traits
    {
      typedef GenVisitor<WeightSet, misc::id_traits> Visitor;
      typedef GenVisitor<WeightSet, misc::constify_traits> ConstVisitor;
    };

  } // rat_exp
} // vcsn

# include <core/rat_exp/visitor.hxx>

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HH_
