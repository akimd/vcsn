#pragma once

#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  /// Star height of an expression.
  template <typename ExpSet>
  unsigned
  star_height(const ExpSet& es, const typename ExpSet::value_t& e);

  namespace detail
  {
    template <typename ExpSet>
    class star_height_visitor
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using self_t = star_height_visitor;
      using expression_t = typename expressionset_t::value_t;
      using context_t = context_t_of<expressionset_t>;
      using node_t = typename super_t::node_t;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "star_height"; }

      star_height_visitor(const expressionset_t& rs)
        : rs_{rs}
      {}

      /// The star height of \a v.
      unsigned operator()(const expression_t& v)
      {
        height_ = 0;
        return recurse_(v);
      }

    private:
      /// Easy recursion: the star height of \a v, saving height_.
      unsigned recurse_(const expression_t& v)
      {
        unsigned res = 0;
        std::swap(res, height_);
        v->accept(*this);
        std::swap(res, height_);
        return res;
      }

      VCSN_RAT_VISIT(add, v)          { visit_(v); }
      VCSN_RAT_VISIT(atom,)           {}
      VCSN_RAT_VISIT(complement, v)   { visit_(v); }
      VCSN_RAT_VISIT(compose, v)      { visit_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { visit_(v); }
      VCSN_RAT_VISIT(infiltration, v) { visit_(v); }
      VCSN_RAT_VISIT(ldiv, v)         { visit_(v); }
      VCSN_RAT_VISIT(lweight, v)      { v.sub()->accept(*this); }
      VCSN_RAT_VISIT(one,)            {}
      VCSN_RAT_VISIT(prod, v)         { visit_(v); }
      VCSN_RAT_VISIT(rweight, v)      { v.sub()->accept(*this); }
      VCSN_RAT_VISIT(shuffle, v)      { visit_(v); }
      VCSN_RAT_VISIT(star, v)         { visit_(v); ++height_; }
      VCSN_RAT_VISIT(transposition, v){ visit_(v); }
      VCSN_RAT_VISIT(zero,)           {}

      template <rat::type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;

      /// Traverse unary node.
      template <rat::exp::type_t Type>
      void visit_(const unary_t<Type>& v)
      {
        height_ = recurse_(v.sub());
      }

      template <rat::type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;

      /// Traverse variadic node.
      template <rat::type_t Type>
      void visit_(const variadic_t<Type>& n)
      {
        height_ = 0;
        // The height of an n-ary is the max of its heights.
        for (const auto& c : n)
          height_ = std::max(height_, recurse_(c));
      }

      using tuple_t = typename super_t::tuple_t;

    private:
      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// The case of tape I.
        template <size_t I>
        auto tape_(const tuple_t& v)
        {
          return star_height(project<I>(visitor_.rs_), std::get<I>(v.sub()));
        }

        /// Info all the tapes.
        template <size_t... I>
        auto tapes_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return std::max({tape_<I>(v)...});
        }

        /// Entry point.
        auto operator()(const tuple_t& v)
        {
          return tapes_(v, labelset_t_of<context_t>::indices);
        }
        self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        unsigned operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        height_ = std::max(height_, visit_tuple<>{*this}(v));
      }

      /// The expressionset.
      const expressionset_t& rs_;
      /// The current star height.
      unsigned height_;
    };
  } // namespace detail


  template <typename ExpSet>
  unsigned
  star_height(const ExpSet& es, const typename ExpSet::value_t& e)
  {
    auto s = detail::star_height_visitor<ExpSet>{es};
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
        return ::vcsn::star_height(e.valueset(), e.value());
      }
    }
  }
} // namespace vcsn
