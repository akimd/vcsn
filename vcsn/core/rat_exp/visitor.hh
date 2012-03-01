#ifndef VCSN_CORE_RAT_EXP_VISITOR_HH_
# define VCSN_CORE_RAT_EXP_VISITOR_HH_

# include <misc/static.hh>
# include <core/rat_exp/node.fwd.hh>

namespace vcsn {
  namespace rat_exp {

    template<template<class> class ConstNess>
    class GenVisitor
    {
    public:
      virtual void visit(typename ConstNess<exp>::type &           v);
      virtual void visit(typename ConstNess<concat>::type &        v);
      virtual void visit(typename ConstNess<plus>::type &          v);
      virtual void visit(typename ConstNess<kleene>::type &        v);
      virtual void visit(typename ConstNess<one>::type &           v);
      virtual void visit(typename ConstNess<zero>::type &          v);
      virtual void visit(typename ConstNess<word>::type &          v);
      virtual void visit(typename ConstNess<left_weight>::type &   v);
      virtual void visit(typename ConstNess<right_weight>::type &  v);
    };

    typedef GenVisitor<misc::id_traits> visitor;
    typedef GenVisitor<misc::constify_traits> ConstVisitor;

  } // !rat_exp
} // !vcsn

# include <core/rat_exp/visitor.hxx>

#endif // !VCSN_CORE_RAT_EXP_VISITOR_HH_
