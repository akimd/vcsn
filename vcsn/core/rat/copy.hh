#pragma once

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
    template <typename InExpSet, typename OutExpSet>
    class copy_impl
      : public InExpSet::const_visitor
    {
    public:
      using in_expressionset_t = InExpSet;
      using out_expressionset_t = OutExpSet;
      using super_t = typename in_expressionset_t::const_visitor;
      using self_t = copy_impl;

      using in_context_t = context_t_of<in_expressionset_t>;
      using in_expression_t = typename in_expressionset_t::value_t;
      using out_expression_t = typename out_expressionset_t::value_t;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;

      copy_impl(const in_expressionset_t& in_rs,
                const out_expressionset_t& out_rs)
        : in_rs_{in_rs}
        , out_rs_{out_rs}
      {}

      /// Entry point: copy/convert \a v.
      out_expression_t
      operator()(const in_expression_t& v)
      {
        return rec_(v);
      }

    protected:
      /// Easy recursion.
      out_expression_t rec_(const in_expression_t& v)
      {
        v->accept(*this);
        return res_;
      }

      /// Factor the handling of unary operations.
      template <exp::type_t Type, typename Fun>
      void
      rec_(const unary_t<Type>& v, Fun&& fun)
      {
        // FIXME: C++17: invoke.
        res_ = (out_rs_.*fun)(rec_(v.sub()));
      }

      /// Factor the handling of n-ary operations.
      template <exp::type_t Type, typename Fun>
      void
      rec_(const variadic_t<Type>& v, Fun&& fun)
      {
        auto res = rec_(v.head());
        for (const auto& c: v.tail())
          // FIXME: C++17: invoke.
          res = (out_rs_.*fun)(res, rec_(c));
        res_ = std::move(res);
      }

      using ors_t = out_expressionset_t;
      VCSN_RAT_VISIT(add, v)          { rec_(v, &ors_t::add); }
      VCSN_RAT_VISIT(complement, v)   { rec_(v, &ors_t::complement); }
      VCSN_RAT_VISIT(compose, v)      { rec_(v, &ors_t::compose); }
      VCSN_RAT_VISIT(conjunction, v)  { rec_(v, &ors_t::conjunction); }
      VCSN_RAT_VISIT(infiltrate, v)   { rec_(v, &ors_t::infiltrate); }
      VCSN_RAT_VISIT(ldivide, v)      { rec_(v, &ors_t::ldivide); }
      VCSN_RAT_VISIT(one,)            { res_ = out_rs_.one(); }
      using bin_t =
        out_expression_t (ors_t::*)(const out_expression_t&,
                                    const out_expression_t&) const;
      VCSN_RAT_VISIT(mul, v)          { rec_(v,
                                             static_cast<bin_t>(&ors_t::mul)); }
      VCSN_RAT_VISIT(shuffle, v)      { rec_(v, &ors_t::shuffle); }
      VCSN_RAT_VISIT(star, v)         { rec_(v, &ors_t::star); }
      VCSN_RAT_VISIT(transposition, v){ rec_(v, &ors_t::transposition); }
      VCSN_RAT_VISIT(zero,)           { res_ = out_rs_.zero(); }

      VCSN_RAT_VISIT(atom, v)
      {
        res_ = out_rs_.atom(out_rs_.labelset()->conv(*in_rs_.labelset(),
                                                     v.value()));
      }

      VCSN_RAT_VISIT(name, v)
      {
        res_ = out_rs_.name(rec_(v.sub()), v.name_get());
      }

      VCSN_RAT_VISIT(lweight, v)
      {
        res_ = out_rs_.lweight(out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                         v.weight()),
                               rec_(v.sub()));
      }

      VCSN_RAT_VISIT(rweight, v)
      {
        res_ = out_rs_.rweight(rec_(v.sub()),
                               out_rs_.weightset()->conv(*in_rs_.weightset(),
                                                         v.weight()));
      }

      /*---------.
      | tuple.   |
      `---------*/

      using tuple_t = typename super_t::tuple_t;

      template <typename = void>
      struct visit_tuple
      {
        /// Copy one tape.
        template <size_t I>
        auto work_(const tuple_t& v)
        {
          return rat::copy(detail::project<I>(self_.in_rs_),
                           detail::project<I>(self_.out_rs_),
                           std::get<I>(v.sub()));
        }

        /// Copy all the tapes.
        template <size_t... I>
        out_expression_t work_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return self_.out_rs_.tuple(work_<I>(v)...);
        }

        /// Entry point.
        out_expression_t operator()(const tuple_t& v)
        {
          return work_(v, labelset_t_of<in_context_t>::indices);
        }
        self_t& self_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        detail::static_if<in_context_t::is_lat>
          ([this](auto&& v){ res_ = visit_tuple<decltype(v)>{*this}(v); })
          (v);
      }

      /// Expressionset to decode the input value.
      const in_expressionset_t& in_rs_;
      /// Expressionset to build the output value.
      const out_expressionset_t& out_rs_;
      /// Output value, under construction.
      out_expression_t res_;
    };

    template <typename InExpSet, typename OutExpSet>
    typename OutExpSet::value_t
    copy(const InExpSet& in_rs, const OutExpSet& out_rs,
         const typename InExpSet::value_t& v)
    {
      auto copy = copy_impl<InExpSet, OutExpSet>{in_rs, out_rs};
      try
        {
          return copy(v);
        }
      catch (const std::runtime_error& e)
        {
          raise(e,
                "  while converting expression ",
                str_quote(to_string(in_rs, v)),
                " to ", to_string(out_rs));
        }
    }
  } // namespace rat
} // namespace vcsn
