#pragma once

#include <iostream>

#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {
    /// Functor to project a rational expression.
    ///
    /// \tparam InExpSet  the input expressionset type
    /// \tparam Tape      the selected tape
    template <typename InExpSet, size_t Tape>
    class project_impl
      : public InExpSet::const_visitor
    {
    public:
      using in_expressionset_t = InExpSet;
      using in_context_t = context_t_of<in_expressionset_t>;
      using out_expressionset_t
        = typename in_expressionset_t::template project_t<Tape>;
      using super_t = typename in_expressionset_t::const_visitor;
      using self_t = project_impl;

      using in_value_t = typename in_expressionset_t::value_t;
      using out_value_t = typename out_expressionset_t::value_t;
      using node_t = typename super_t::node_t;
      using inner_t = typename super_t::inner_t;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      using leaf_t = typename super_t::leaf_t;

      project_impl(const in_expressionset_t& in_rs)
        : in_rs_(in_rs)
        , out_rs_(::vcsn::detail::project<Tape>(in_rs_.context()))
      {}

      /// Entry point: print \a v.
      out_value_t
      operator()(const in_value_t& v)
      {
        return project(v);
      }

    private:
      /// Easy recursion.
      out_value_t project(const in_value_t& v)
      {
        v->accept(*this);
        return res_;
      }

      /// Factor the handling of unary operations.
      template <exp::type_t Type>
      void
      project_(const unary_t<Type>& v)
      {
        using out_unary_t
          = typename out_expressionset_t::template unary_t<Type>;
        res_ = std::make_shared<out_unary_t>(project(v.sub()));
      }

      /// Factor the handling of n-ary operations.
      template <exp::type_t Type, typename Fun>
      void
      project_(const variadic_t<Type>& v, Fun&& fun)
      {
        // Be sure to apply the identities: merely projecting on 0 in
        // `a|b+a|b` would result in `a+a`.
        auto res = project(v.head());
        for (const auto& c: v.tail())
          // FIXME: C++17: invoke.
          res = (out_rs_.*fun)(res, project(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(complement, v)   { project_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { project_(v, &out_expressionset_t::conjunction); }
      VCSN_RAT_VISIT(infiltration, v) { project_(v, &out_expressionset_t::infiltration); }
      VCSN_RAT_VISIT(ldiv, v)         { project_(v, &out_expressionset_t::ldiv); }
      VCSN_RAT_VISIT(one,)            { res_ = out_rs_.one(); }
      // I do not understand why I need the cast just for this one,
      // but GCC and Clang agree.
      using bin_t =
        out_value_t
        (out_expressionset_t::*)(const out_value_t&, const out_value_t&) const;
      VCSN_RAT_VISIT(prod, v)         { project_(v,
                                                 static_cast<bin_t>(&out_expressionset_t::mul)); }
      VCSN_RAT_VISIT(shuffle, v)      { project_(v, &out_expressionset_t::shuffle); }
      VCSN_RAT_VISIT(star, v)         { project_(v); }
      VCSN_RAT_VISIT(sum, v)          { project_(v, &out_expressionset_t::add); }
      VCSN_RAT_VISIT(transposition, v){ project_(v); }
      VCSN_RAT_VISIT(zero,)           { res_ = out_rs_.zero(); }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = out_rs_.atom(std::get<Tape>(v.value()));
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        res_ = out_rs_.lmul(v.weight(),
                            project(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        res_ = out_rs_.rmul(project(v.sub()),
                            v.weight());
      }

      /*---------.
      | tuple.   |
      `---------*/

      using tuple_t = typename super_t::tuple_t;

      template <bool = in_context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Entry point.
        out_value_t operator()(const tuple_t& v)
        {
          return std::get<Tape>(v.sub());
        }
        self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        out_value_t operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        res_ = visit_tuple<>{*this}(v);
      }

      /// expressionset to decode the input value.
      const in_expressionset_t& in_rs_;
      /// expressionset to build the output value.
      out_expressionset_t out_rs_;
      /// Output value, under construction.
      out_value_t res_;
    };


    /// Project a rational expression.
    ///
    /// \tparam ExpSet  the expressionset type
    /// \tparam Tape    the selected tape
    template <size_t Tape, typename InExpSet>
    auto
    project(const InExpSet& in_rs, const typename InExpSet::value_t& v)
      -> typename project_impl<InExpSet, Tape>::out_value_t
    {
      auto p = project_impl<InExpSet, Tape>{in_rs};
      return p(v);
    }
  } // namespace rat
} // namespace vcsn
