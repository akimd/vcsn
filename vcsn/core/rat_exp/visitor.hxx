#ifndef VCSN_CORE_RAT_EXP_VISITOR_HXX_
# define VCSN_CORE_RAT_EXP_VISITOR_HXX_

# include <core/rat_exp/visitor.hh>
# include <core/rat_exp/node.hh>
# include <cassert>

namespace vcsn
{
  namespace rat_exp
  {
#define VISIT(Type, Name)                                       \
    template<class WeightSet, template<class> class ConstNess>  \
    inline                                                      \
    void                                                        \
    GenVisitor<WeightSet, ConstNess>                            \
    ::visit(typename ConstNess<Type<WeightSet>>::type& Name)


    VISIT(RatExpConcat, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(RatExpPlus, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(RatExpKleene, v)
    {
      if (auto sub = v.get_sub())
        sub->accept(*this);
    }

    VISIT(RatExpOne, )
    {}

    VISIT(RatExpZero, )
    {}

    VISIT(RatExpWord, )
    {}

#undef VISIT

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HXX_
