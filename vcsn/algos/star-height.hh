#pragma once

#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename ExpSet>
    class star_heighter
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using node_t = typename super_t::node_t;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "star_height"; }

      /// Entry point: return the size of \a v.
      unsigned
      operator()(const node_t& v)
      {
        height_ = 0;
        v.accept(*this);
        return height_;
      }

      /// Entry point: return the size of \a v.
      unsigned
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

      VCSN_RAT_VISIT(atom,)           {}
      VCSN_RAT_VISIT(complement, v)   { visit_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_(v); }
      VCSN_RAT_VISIT(infiltration, v) { visit_(v); }
      VCSN_RAT_VISIT(ldiv, v)         { visit_(v); }
      VCSN_RAT_VISIT(lweight, v)      { v.sub()->accept(*this); }
      VCSN_RAT_VISIT(one,)            {}
      VCSN_RAT_VISIT(prod, v)         { visit_(v); }
      VCSN_RAT_VISIT(rweight, v)      { v.sub()->accept(*this); }
      VCSN_RAT_VISIT(shuffle, v)      { visit_(v); }
      VCSN_RAT_VISIT(star, v)         { ++height_; visit_(v); }
      VCSN_RAT_VISIT(sum, v)          { visit_(v); }
      VCSN_RAT_VISIT(transposition, v){ visit_(v); }
      VCSN_RAT_VISIT(zero,)           {}

      template <rat::type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;

      /// Traverse unary node.
      template <rat::exp::type_t Type>
      void visit_(const unary_t<Type>& v)
      {
        v.sub()->accept(*this);
      }

      template <rat::type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;

      /// Traverse variadic node.
      template <rat::type_t Type>
      void visit_(const variadic_t<Type>& n)
      {
        /* The height of an n-ary is the max of the n heights. */
        auto max = height_;
        auto initial = height_;
        for (auto child : n)
          {
            height_ = initial;
            child->accept(*this);
            if (max < height_)
              max = height_;
          }
        height_ = max;
      }

      using tuple_t = typename super_t::tuple_t;
      virtual void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
      }

      /// The current star height.
      unsigned height_;
    };
  } // namespace detail


  /// Star height of an expression.
  template <typename ExpSet>
  inline
  unsigned
  star_height(const typename ExpSet::value_t& e)
  {
    detail::star_heighter<ExpSet> s;
    return s(e);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      unsigned
      star_height(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return ::vcsn::star_height<ExpSet>(e.value());
      }
    }
  }
} // namespace vcsn
