#pragma once

#include <iostream>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {

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
      using in_value_t = typename in_expressionset_t::value_t;
      using out_value_t = typename out_expressionset_t::value_t;
      using super_t = typename in_expressionset_t::const_visitor;
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

      /// Factor the copy of n-ary operations.
      template <exp::type_t Type>
      void
      copy_(const unary_t<Type>& v)
      {
        using out_unary_t
          = typename out_expressionset_t::template unary_t<Type>;
        res_ = std::make_shared<out_unary_t>(copy(v.sub()));
      }

      /// Factor the copy of n-ary operations.
      template <exp::type_t Type>
      void
      copy_(const variadic_t<Type>& v)
      {
        using out_variadic_t
          = typename out_expressionset_t::template variadic_t<Type>;
        typename out_expressionset_t::values_t sub;
        for (const auto& s: v)
          sub.emplace_back(copy(s));
        res_ = std::make_shared<out_variadic_t>(sub);
      }

      VCSN_RAT_VISIT(complement, v)   { copy_(v); }
      VCSN_RAT_VISIT(conjunction, v)  { copy_(v); }
      VCSN_RAT_VISIT(infiltration, v) { copy_(v); }
      VCSN_RAT_VISIT(ldiv, v)         { copy_(v); }
      VCSN_RAT_VISIT(one,)            { res_ = out_rs_.one(); }
      VCSN_RAT_VISIT(prod, v)         { copy_(v); }
      VCSN_RAT_VISIT(shuffle, v)      { copy_(v); }
      VCSN_RAT_VISIT(star, v)         { copy_(v); }
      VCSN_RAT_VISIT(sum, v)          { copy_(v); }
      VCSN_RAT_VISIT(transposition, v){ copy_(v); }
      VCSN_RAT_VISIT(zero,)           { res_ = out_rs_.zero(); }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = out_rs_.atom(out_rs_.labelset()->conv(*in_rs_.labelset(),
                                                     v.value()));
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        res_ = out_rs_.lmul(out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                      v.weight()),
                            copy(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        res_ = out_rs_.rmul(copy(v.sub()),
                            out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                      v.weight()));
      }

      using tuple_t = typename super_t::tuple_t;
      virtual void visit(const tuple_t&, std::true_type) override
      {
        abort();
      }

      /// expressionset to decode the input value.
      const in_expressionset_t& in_rs_;
      /// expressionset to build the output value.
      const out_expressionset_t& out_rs_;
      /// Output value, under construction.
      out_value_t res_;
    };

    template <typename InExpSet, typename OutExpSet = InExpSet>
    typename OutExpSet::value_t
    copy(const InExpSet& in_rs, const OutExpSet& out_rs,
         const typename InExpSet::value_t& v)
    {
      auto copy = copier<InExpSet, OutExpSet>{in_rs, out_rs};
      return copy(v);
    }

  } // namespace rat
} // namespace vcsn
