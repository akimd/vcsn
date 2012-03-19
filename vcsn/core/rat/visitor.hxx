#ifndef VCSN_CORE_RAT_VISITOR_HXX_
# define VCSN_CORE_RAT_VISITOR_HXX_

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
    gen_visitor<WeightSet, ConstNess>                            \
    ::visit(typename ConstNess<Type<WeightSet>>::type& Name)


    VISIT(concat, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(plus, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(kleene, v)
    {
      if (auto sub = v.get_sub())
        sub->accept(*this);
    }

    VISIT(one, )
    {}

    VISIT(zero, )
    {}

    VISIT(word, )
    {}

#undef VISIT

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_VISITOR_HXX_
