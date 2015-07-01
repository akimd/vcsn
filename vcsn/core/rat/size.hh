#pragma once

#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {
    template <typename ExpSet>
    class sizer
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

      VCSN_RAT_VISIT(atom,)           { ++size_; }
      VCSN_RAT_VISIT(complement, v)   { visit_unary(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_variadic(v); }
      VCSN_RAT_VISIT(ldiv, v)         { visit_variadic(v); }
      VCSN_RAT_VISIT(lweight, v)      { ++size_; v.sub()->accept(*this); }
      VCSN_RAT_VISIT(one,)            { ++size_; }
      VCSN_RAT_VISIT(prod, v)         { visit_variadic(v); };
      VCSN_RAT_VISIT(rweight, v)      { ++size_; v.sub()->accept(*this); }
      VCSN_RAT_VISIT(shuffle, v)      { visit_variadic(v); };
      VCSN_RAT_VISIT(star, v)         { visit_unary(v); }
      VCSN_RAT_VISIT(sum, v)          { visit_variadic(v); };
      VCSN_RAT_VISIT(transposition, v){ visit_unary(v); }
      VCSN_RAT_VISIT(zero,)           { ++size_; }

      using tuple_t = typename super_t::tuple_t;
      template <bool = is_two_tapes_t<context_t>{},
                typename Dummy = void>
      struct visit_tuple
      {
        size_t operator()(const tuple_t& v)
        {
          auto res = size_t{0};
          using ctx0_t = detail::focus_context<0, context_t>;
          using rs0_t = expressionset<ctx0_t>;
          res += size<rs0_t>(std::get<0>(v.sub()));

          using ctx1_t = detail::focus_context<1, context_t>;
          using rs1_t = expressionset<ctx1_t>;
          res += size<rs1_t>(std::get<1>(v.sub()));
          return res;
        }
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        size_t operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        visit_tuple<>{}(v);
      }

      /// Traverse unary node.
      template <rat::exp::type_t Type>
      void visit_unary(const unary_t<Type>& v)
      {
        ++size_;
        v.sub()->accept(*this);
      }

      /// Traverse variadic node.
      template <rat::exp::type_t Type>
      void visit_variadic(const variadic_t<Type>& v)
      {
        // One operator bw each argument.
        size_ += v.size() - 1;
        for (auto child : v)
          child->accept(*this);
      }

      size_t size_;
    };

    template <typename ExpSet>
    size_t size(const typename ExpSet::value_t& r)
    {
      auto s = sizer<ExpSet>{};
      return s(r);
    }

  } // namespace rat
} // namespace vcsn
