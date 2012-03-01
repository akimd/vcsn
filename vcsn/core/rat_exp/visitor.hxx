#ifndef VCSN_CORE_RAT_EXP_VISITOR_HXX_
# define VCSN_CORE_RAT_EXP_VISITOR_HXX_

# include <core/rat_exp/visitor.hh>
# include <core/rat_exp/node.hh>
# include <cassert>

namespace vcsn {
  namespace rat_exp {

    // template<template<class> class ConstNess>
    // GenVisitor<ConstNess>::GenVisitor()
    // { }

    // template<template<class> class ConstNess>
    // GenVisitor<ConstNess>::~GenVisitor()
    // { }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::exp>::type & v)
    {
      assert(false);
    }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::concat>::type & v)
    {
      for(auto t : v)
        t->accept(*this);
    }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::plus>::type & v)
    {
      for(auto t : v)
        t->accept(*this);
    }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::kleene>::type & v)
    {
      typename ConstNess<exp>::type *sub_exp = v.get_sub();

      if(sub_exp!= nullptr)
        sub_exp->accept(*this);
    }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::one>::type & v)
    { }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::zero>::type & v)
    { }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::word>::type & v)
    { }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::left_weight>::type & v)
    {
      typename ConstNess<exp>::type *sub_node = v.get_exp();
      if(sub_node != nullptr)
        sub_node->accept(*this);
    }

    template<template<class> class ConstNess>
    inline
    void
    GenVisitor<ConstNess>::visit(typename ConstNess<vcsn::rat_exp::right_weight>::type & v)
    {
      typename ConstNess<exp>::type *sub_node = v.get_exp();
      if(sub_node != nullptr)
        sub_node->accept(*this);
    }

  } // !rat_exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HXX_
