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
#define VISIT(Type)                                                     \
      virtual void visit(typename ConstNess<Type<WeightSet> >::type& v)

      VISIT(RatExpConcat);
      VISIT(RatExpPlus);
      VISIT(RatExpKleene);
      VISIT(RatExpOne);
      VISIT(RatExpZero);
      VISIT(RatExpWord);
#undef VISIT
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
