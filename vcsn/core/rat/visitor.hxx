#ifndef VCSN_CORE_RAT_EXP_VISITOR_HXX_
# define VCSN_CORE_RAT_EXP_VISITOR_HXX_

# include <core/rat/visitor.hh>
# include <core/rat/node.hh>
# include <cassert>

namespace vcsn
{
  namespace rat
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

  } // rat
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HXX_
