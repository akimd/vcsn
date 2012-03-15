#ifndef VCSN_CORE_RAT_EXP_VISITOR_HXX_
# define VCSN_CORE_RAT_EXP_VISITOR_HXX_

# include <core/rat_exp/visitor.hh>
# include <core/rat_exp/node.hh>
# include <cassert>

namespace vcsn
{
  namespace rat_exp
  {

    template<class WeightSet, template<class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpNode<WeightSet> >::type &)
    {
      assert(false);
    }

    template <class WeightSet, template <class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpConcat<WeightSet> >::type& v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    template <class WeightSet, template <class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpPlus<WeightSet> >::type& v)
    {
      for (auto t : v)
        t->accept(*this);
    }

    template <class WeightSet, template <class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpKleene<WeightSet> >::type& v)
    {
      typename ConstNess<RatExpNode<WeightSet> >::type *sub_exp = v.getSubNode();

      if (sub_exp!= nullptr)
        sub_exp->accept(*this);
    }

    template <class WeightSet, template <class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpOne<WeightSet> >::type &)
    { }

    template <class WeightSet, template <class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpZero<WeightSet> >::type &)
    { }

    template <class WeightSet, template <class> class ConstNess>
    inline
    void
    GenVisitor<WeightSet, ConstNess>::visit(typename ConstNess<RatExpWord<WeightSet> >::type &)
    { }

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HXX_
