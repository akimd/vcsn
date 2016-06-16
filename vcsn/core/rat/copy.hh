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

    /// Copy/convert a rational expression.
    ///
    /// \tparam InExpSet   the input expressionset type.
    /// \tparam OutExpSet  the output expressionset type.
    template <typename InExpSet, typename OutExpSet = InExpSet>
    typename OutExpSet::value_t
    copy(const InExpSet& in_rs, const OutExpSet& out_rs,
         const typename InExpSet::value_t& v);

    /// Functor to copy/convert a rational expression.
    ///
    /// \tparam InExpSet   the input expressionset type.
    /// \tparam OutExpSet  the output expressionset type.
    template <typename InExpSet, typename OutExpSet = InExpSet>
    class copier
      : public InExpSet::const_visitor
    {
    public:
      using in_expressionset_t = InExpSet;
      using out_expressionset_t = OutExpSet;
      using super_t = typename in_expressionset_t::const_visitor;
      using self_t = copier;

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

      copier(const in_expressionset_t& in_rs, const out_expressionset_t& out_rs)
        : in_rs_(in_rs)
        , out_rs_(out_rs)
      {}

      /// Entry point: print \a v.
      out_value_t
      operator()(const in_value_t& v)
      {
        return copy(v);
      }

    private:
      /// Easy recursion.
      out_value_t copy(const in_value_t& v)
      {
        v->accept(*this);
        return res_;
      }

      /// Factor the handling of unary operations.
      template <exp::type_t Type>
      void
      copy_(const unary_t<Type>& v)
      {
        using out_unary_t
          = typename out_expressionset_t::template unary_t<Type>;
        res_ = std::make_shared<out_unary_t>(copy(v.sub()));
      }

      /// Factor the handling of n-ary operations.
      template <exp::type_t Type>
      void
      copy_(const variadic_t<Type>& v)
      {
        using out_variadic_t
          = typename out_expressionset_t::template variadic_t<Type>;
        auto sub = typename out_expressionset_t::values_t{};
        for (const auto& s: v)
          sub.emplace_back(copy(s));
        res_ = std::make_shared<out_variadic_t>(sub);
      }

      VCSN_RAT_VISIT(add, v)          { copy_(v); }
      VCSN_RAT_VISIT(complement, v)   { copy_(v); }
      VCSN_RAT_VISIT(compose, v)      { copy_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { copy_(v); }
      VCSN_RAT_VISIT(infiltrate, v)   { copy_(v); }
      VCSN_RAT_VISIT(ldiv, v)         { copy_(v); }
      VCSN_RAT_VISIT(mul, v)          { copy_(v); }
      VCSN_RAT_VISIT(one,)            { res_ = out_rs_.one(); }
      VCSN_RAT_VISIT(shuffle, v)      { copy_(v); }
      VCSN_RAT_VISIT(star, v)         { copy_(v); }
      VCSN_RAT_VISIT(transposition, v){ copy_(v); }
      VCSN_RAT_VISIT(zero,)           { res_ = out_rs_.zero(); }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = out_rs_.atom(out_rs_.labelset()->conv(*in_rs_.labelset(),
                                                     v.value()));
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        res_ = out_rs_.lweight(out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                         v.weight()),
                               copy(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        res_ = out_rs_.rweight(copy(v.sub()),
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
        /// Copy one tape.
        template <size_t I>
        auto work_(const tuple_t& v)
        {
          return rat::copy(detail::project<I>(visitor_.in_rs_),
                           detail::project<I>(visitor_.out_rs_),
                           std::get<I>(v.sub()));
        }

        /// Copy all the tapes.
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
    copy(const InExpSet& in_rs, const OutExpSet& out_rs,
         const typename InExpSet::value_t& v)
    {
      auto copy = copier<InExpSet, OutExpSet>{in_rs, out_rs};
      return copy(v);
    }
  } // namespace rat
} // namespace vcsn
