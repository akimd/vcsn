#pragma once

#include <iostream>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>

#include <vcsn/algos/project.hh> // project.

namespace vcsn
{
  namespace rat
  {

    /// Partial identity of a rational expression.
    ///
    /// \tparam InExpSet   the input expressionset type.
    /// \tparam OutExpSet  the output expressionset type.
    template <typename InExpSet,
              typename OutExpSet = InExpSet>
    typename OutExpSet::value_t
    partial_identity(const InExpSet& in_rs, const OutExpSet& out_rs,
                     const typename InExpSet::value_t& v);

    /// Functor to partial_identity/convert a rational expression.
    ///
    /// \tparam InExpSet   the input expressionset type.
    /// \tparam OutExpSet  the output expressionset type.
    template <typename InExpSet, typename OutExpSet = InExpSet>
    class partial_identity_impl
      : public InExpSet::const_visitor
    {
    public:
      using in_expressionset_t = InExpSet;
      using out_expressionset_t = OutExpSet;
      using super_t = typename in_expressionset_t::const_visitor;
      using self_t = partial_identity_impl;

      using in_context_t = context_t_of<in_expressionset_t>;
      using in_value_t = typename in_expressionset_t::value_t;
      using out_value_t = typename out_expressionset_t::value_t;
      using node_t = typename super_t::node_t;
      using inner_t = typename super_t::inner_t;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      using leaf_t = typename super_t::leaf_t;

      using out_expression_t = typename out_expressionset_t::value_t;

      partial_identity_impl(const in_expressionset_t& in_rs,
                            const out_expressionset_t& out_rs)
        : in_rs_{in_rs}
        , out_rs_{out_rs}
      {}

      /// Entry point: print \a v.
      out_value_t
      operator()(const in_value_t& v)
      {
        return recurse(v);
      }

    private:
      /// Easy recursion.
      out_value_t recurse(const in_value_t& v)
      {
        v->accept(*this);
        return res_;
      }

      /// Factor the handling of unary operations.
      template <exp::type_t Type>
      void
      recurse_(const unary_t<Type>& v)
      {
        using out_unary_t
          = typename out_expressionset_t::template unary_t<Type>;
        res_ = std::make_shared<out_unary_t>(recurse(v.sub()));
      }

      /// Factor the handling of n-ary operations.
      template <exp::type_t Type, typename Fun>
      void
      recurse_(const variadic_t<Type>& v, Fun&& fun)
      {
        using out_variadic_t
          = typename out_expressionset_t::template variadic_t<Type>;
        auto sub = typename out_expressionset_t::values_t{};
        for (const auto& s: v)
          sub.emplace_back(recurse(s));
        res_ = std::make_shared<out_variadic_t>(sub);
      }

      VCSN_RAT_VISIT(add, v)          { recurse_(v, &out_expressionset_t::add); }
      VCSN_RAT_VISIT(complement, v)   { recurse_(v); }
      VCSN_RAT_VISIT(compose, v)      { recurse_(v, &out_expressionset_t::compose); }
      VCSN_RAT_VISIT(conjunction, v)  { recurse_(v, &out_expressionset_t::conjunction); }
      VCSN_RAT_VISIT(infiltrate, v)   { recurse_(v, &out_expressionset_t::infiltrate); }
      VCSN_RAT_VISIT(ldivide, v)      { recurse_(v, &out_expressionset_t::ldivide); }
      using bin_t =
        out_expression_t
        (out_expressionset_t::*)(const out_expression_t&, const out_expression_t&) const;
      VCSN_RAT_VISIT(mul, v)          { recurse_(v,
                                                 static_cast<bin_t>(&out_expressionset_t::mul)); }
      VCSN_RAT_VISIT(one,)            { res_ = out_rs_.one(); }
      VCSN_RAT_VISIT(shuffle, v)      { recurse_(v, &out_expressionset_t::shuffle); }
      VCSN_RAT_VISIT(star, v)         { recurse_(v); }
      VCSN_RAT_VISIT(transposition, v){ recurse_(v); }
      VCSN_RAT_VISIT(zero,)           { res_ = out_rs_.zero(); }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = out_rs_.atom(out_rs_.labelset()->tuple(v.value(), v.value()));
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        res_ = out_rs_.lweight(out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                         v.weight()),
                               recurse(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        res_ = out_rs_.rweight(recurse(v.sub()),
                               out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                         v.weight()));
      }

      /*---------.
      | tuple.   |
      `---------*/

      using tuple_t = typename super_t::tuple_t;

      template <bool = in_context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Partial_Identity one tape.
        template <size_t I>
        auto work_(const tuple_t& v)
        {
          return rat::partial_identity(detail::project<I>(visitor_.in_rs_),
                           detail::project<I>(visitor_.out_rs_),
                           std::get<I>(v.sub()));
        }

        /// Partial_Identity all the tapes.
        template <size_t... I>
        out_value_t work_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return visitor_.out_rs_.tuple(work_<I>(v)...);
        }

        /// Entry point.
        out_value_t operator()(const tuple_t& v)
        {
          return work_(v, labelset_t_of<in_context_t>::indices);
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
      const out_expressionset_t& out_rs_;
      /// Output value, under construction.
      out_value_t res_;
    };

    template <typename InExpSet, typename OutExpSet>
    typename OutExpSet::value_t
    partial_identity(const InExpSet& in_rs, const OutExpSet& out_rs,
                     const typename InExpSet::value_t& v)
    {
      auto pid = partial_identity_impl<InExpSet, OutExpSet>{in_rs, out_rs};
      return pid(v);
    }
  } // namespace rat
} // namespace vcsn
