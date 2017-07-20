#pragma once

#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {
    /// Functor to compute the size of a rational expression.
    ///
    /// \tparam ExpSet  the expressionset type.
    template <typename ExpSet>
    class sizer
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;

      using context_t = context_t_of<expressionset_t>;
      using weight_t = typename context_t::weightset_t::value_t;
      /// Actual node, without indirection.
      using node_t = typename super_t::node_t;
      /// A shared_ptr to node_t.
      using expression_t = typename node_t::value_t;

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
      operator()(const expression_t& v)
      {
        size_ = 0;
        v->accept(*this);
        return size_;
      }

    private:

      VCSN_RAT_VISIT(add, v)          { visit_(v); };
      VCSN_RAT_VISIT(atom,)           { ++size_; }
      VCSN_RAT_VISIT(complement, v)   { visit_(v); }
      VCSN_RAT_VISIT(compose, v)      { visit_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_(v); }
      VCSN_RAT_VISIT(infiltrate, v)   { visit_(v); };
      VCSN_RAT_VISIT(ldivide, v)      { visit_(v); }
      VCSN_RAT_VISIT(lweight, v)      { ++size_; v.sub()->accept(*this); }
      VCSN_RAT_VISIT(mul, v)          { visit_(v); };
      VCSN_RAT_VISIT(name, v)         { super_t::visit(v); };
      VCSN_RAT_VISIT(one,)            { ++size_; }
      VCSN_RAT_VISIT(rweight, v)      { ++size_; v.sub()->accept(*this); }
      VCSN_RAT_VISIT(shuffle, v)      { visit_(v); };
      VCSN_RAT_VISIT(star, v)         { visit_(v); }
      VCSN_RAT_VISIT(transposition, v){ visit_(v); }
      VCSN_RAT_VISIT(zero,)           { ++size_; }

      using tuple_t = typename super_t::tuple_t;
      template <typename Dummy = void>
      struct visit_tuple
      {
        /// Size for one tape.
        template <size_t I>
        size_t size_(const tuple_t& v)
        {
          using rs_t = typename expressionset_t::template project_t<I>;
          return size<rs_t>(std::get<I>(v.sub()));
        }

        /// Sum of sizes for all tapes.
        template <size_t... I>
        size_t size_(const tuple_t& v, detail::index_sequence<I...>)
        {
          auto res = size_t{0};
          using swallow = int[];
          (void) swallow
          {
            (res += size_<I>(v),
             0)...
          };
          return res;
        }

        /// Entry point.
        size_t operator()(const tuple_t& v)
        {
          return size_(v, labelset_t_of<context_t>::indices);
        }
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        detail::static_if<context_t::is_lat>
          ([this](auto&& v){ size_ += visit_tuple<decltype(v)>{}(v); })
          (v);
      }

      /// Traverse unary node.
      template <rat::exp::type_t Type>
      void visit_(const unary_t<Type>& v)
      {
        ++size_;
        v.sub()->accept(*this);
      }

      /// Traverse variadic node.
      template <rat::exp::type_t Type>
      void visit_(const variadic_t<Type>& v)
      {
        // One operator bw each argument.
        size_ += v.size() - 1;
        for (const auto& child : v)
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
