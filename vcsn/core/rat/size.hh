#pragma once

#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {
    template <typename ExpSet>
    class size
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using context_t = context_t_of<expressionset_t>;
      using weight_t = typename context_t::weightset_t::value_t;
      using super_t = typename expressionset_t::const_visitor;
      using node_t = typename super_t::node_t;
      using inner_t = typename super_t::inner_t;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      using leaf_t = typename super_t::leaf_t;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "size"; }

      /// Entry point: return the size of \a v.
      size_t
      operator()(const node_t& v)
      {
        size_ = 0;
        v.accept(*this);
        return size_;
      }

      /// Entry point: return the size of \a v.
      size_t
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

      VCSN_RAT_VISIT(atom, v);
      VCSN_RAT_VISIT(complement, v)   { visit_unary(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_variadic(v); }
      VCSN_RAT_VISIT(ldiv, v)         { visit_variadic(v); }
      VCSN_RAT_VISIT(lweight, v);
      VCSN_RAT_VISIT(one, v);
      VCSN_RAT_VISIT(prod, v)         { visit_variadic(v); };
      VCSN_RAT_VISIT(rweight, v);
      VCSN_RAT_VISIT(shuffle, v)      { visit_variadic(v); };
      VCSN_RAT_VISIT(star, v)         { visit_unary(v); }
      VCSN_RAT_VISIT(sum, v)          { visit_variadic(v); };
      VCSN_RAT_VISIT(transposition, v){ visit_unary(v); }
      VCSN_RAT_VISIT(zero, v);


      /// Traverse unary node.
      template <rat::exp::type_t Type>
      void visit_unary(const unary_t<Type>& n);

      /// Traverse variadic node.
      template <rat::exp::type_t Type>
      void visit_variadic(const variadic_t<Type>& n);

      size_t size_;
    };
  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/size.hxx>
