#pragma once

#include <iostream>
#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    /// Gather information of the number of the different node types.
    template <typename ExpSet>
    class info
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using context_t = context_t_of<expressionset_t>;
      using node_t = typename expressionset_t::node_t;
      using super_t = typename expressionset_t::const_visitor;

      /// For each node type, count its number of occurrences.
      void operator()(const node_t& v);

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "split"; }

#define DEFINE(Type)                                    \
    public:                                             \
      size_t Type = 0;                                  \
    private:                                            \
      VCSN_RAT_VISIT(Type, v)

      DEFINE(atom);
      DEFINE(complement)  { ++complement;  visit_(v); }
      DEFINE(conjunction) { ++conjunction; visit_(v); }
      DEFINE(ldiv)        { ++ldiv;        visit_(v); }
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod)        { ++prod;        visit_(v);}
      DEFINE(rweight);
      DEFINE(shuffle)     { ++shuffle;     visit_(v); }
      DEFINE(star)        { ++star;        visit_(v); }
      DEFINE(sum)         { ++sum;         visit_(v);  }
      DEFINE(transposition){ ++transposition; visit_(v);  }
      DEFINE(zero);
#undef DEFINE

    private:
      template <type_t Type>
      using unary_t = rat::unary<Type, context_t>;

      /// Factor the visitation of unary nodes.
      template <rat::exp::type_t Type>
      void visit_(const unary_t<Type>& v)
      {
        v.sub()->accept(*this);
      }

      template <type_t Type>
      using variadic_t = rat::variadic<Type, context_t>;

      /// Factor the visitation of variadic nodes.
      template <exp::type_t Type>
      void visit_(const variadic_t<Type>& v)
      {
        for (auto c: v)
          c->accept(*this);
      }
    };

  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/info.hxx>
