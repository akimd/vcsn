#ifndef VCSN_CORE_RAT_VISITOR_HXX_
# define VCSN_CORE_RAT_VISITOR_HXX_

# include <vcsn/core/rat/visitor.hh>
# include <vcsn/core/rat/node.hh>
# include <cassert>

namespace vcsn
{
  namespace rat
  {
#define VISIT(Type, Name)                                       \
    template<class Weight, template<class> class ConstNess>     \
    inline                                                      \
    void                                                        \
    gen_visitor<Weight, ConstNess>                              \
    ::visit(typename ConstNess<Type<Weight>>::type& Name)


    VISIT(prod, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(sum, v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    VISIT(star, v)
    {
      if (auto sub = v.get_sub())
        sub->accept(*this);
    }

    VISIT(one, )
    {}

    VISIT(zero, )
    {}

    VISIT(atom, )
    {}

#undef VISIT

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_VISITOR_HXX_
