#pragma once

#include <vcsn/core/rat/visitor.hh>

namespace vcsn::rat
{
  /// Functor to project a rational expression.
  ///
  /// \tparam InExpSet   the input expressionset type
  /// \tparam OutExpSet  the output expressionset type
  /// \tparam Tape       the selected tape
  template <typename InExpSet, typename OutExpSet, size_t Tape>
  class project_impl
    : public InExpSet::const_visitor
  {
  public:
    using in_expressionset_t = InExpSet;
    using out_expressionset_t = OutExpSet;
    using super_t = typename in_expressionset_t::const_visitor;
    using self_t = project_impl;

    using in_context_t = context_t_of<in_expressionset_t>;
    using in_expression_t = typename in_expressionset_t::value_t;
    using out_expression_t = typename out_expressionset_t::value_t;
    template <type_t Type>
    using unary_t = typename super_t::template unary_t<Type>;
    template <type_t Type>
    using variadic_t = typename super_t::template variadic_t<Type>;

    project_impl(const in_expressionset_t& in_rs,
                 const out_expressionset_t& out_rs)
      : in_rs_{in_rs}
      , out_rs_{out_rs}
    {}

    project_impl(const in_expressionset_t& in_rs)
      : project_impl{in_rs,
                     in_rs.template project<Tape>()}
    {}

    /// Entry point: print \a v.
    out_expression_t
    operator()(const in_expression_t& v)
    {
      return rec_(v);
    }

  private:
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
      res_ = std::invoke(fun, out_rs_, rec_(v.sub()));
    }

    /// Factor the handling of n-ary operations.
    template <exp::type_t Type, typename Fun>
    void
    rec_(const variadic_t<Type>& v, Fun&& fun)
    {
      // Be sure to apply the identities: merely projecting on tape
      // 0 in `a|b+a|c` would result in `a+a`.
      auto res = rec_(v.head());
      for (const auto& c: v.tail())
        res = std::invoke(fun, out_rs_, res, rec_(c));
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
      out_expression_t
      (ors_t::*)(const out_expression_t&, const out_expression_t&) const;
    VCSN_RAT_VISIT(mul, v)          { rec_(v,
                                           static_cast<bin_t>(&ors_t::mul)); }
    VCSN_RAT_VISIT(shuffle, v)      { rec_(v, &ors_t::shuffle); }
    VCSN_RAT_VISIT(star, v)         { rec_(v, &ors_t::star); }
    VCSN_RAT_VISIT(transposition, v){ rec_(v, &ors_t::transposition); }
    VCSN_RAT_VISIT(zero,)           { res_ = out_rs_.zero(); }

    VCSN_RAT_VISIT(atom, v)
    {
      res_ = out_rs_.atom(std::get<Tape>(v.value()));
    }

    VCSN_RAT_VISIT(name, v)
    {
      res_ = out_rs_.name(rec_(v.sub()), v.name_get());
    }

    VCSN_RAT_VISIT(lweight, v)
    {
      res_ = out_rs_.lweight(v.weight(), rec_(v.sub()));
    }

    VCSN_RAT_VISIT(rweight, v)
    {
      res_ = out_rs_.rweight(rec_(v.sub()), v.weight());
    }

    /*---------.
    | tuple.   |
    `---------*/

    using tuple_t = typename super_t::tuple_t;

    template <typename = void>
    struct visit_tuple
    {
      /// Entry point.
      out_expression_t operator()(const tuple_t& v)
      {
        return std::get<Tape>(v.sub());
      }
      self_t& visitor_;
    };

    void visit(const tuple_t& v, std::true_type) override
    {
      if constexpr (in_context_t::is_lat)
        res_ = visit_tuple<decltype(v)>{*this}(v);
    }

    /// expressionset to decode the input value.
    const in_expressionset_t& in_rs_;
    /// expressionset to build the output value.
    out_expressionset_t out_rs_;
    /// Output value, under construction.
    out_expression_t res_;
  };


  /// Project a rational expression.
  ///
  /// \tparam Tape      the selected tape
  /// \tparam InExpSet  the input expressionset type
  /// \tparam OutExpSet the output expressionset type
  template <size_t Tape, typename InExpSet, typename OutExpSet>
  auto
  project(const InExpSet& in_rs, const OutExpSet& out_rs,
          const typename InExpSet::value_t& v)
    -> typename OutExpSet::value_t
  {
    auto p = project_impl<InExpSet, OutExpSet, Tape>{in_rs, out_rs};
    return p(v);
  }

  /// Project a rational expression.
  ///
  /// \tparam Tape     the selected tape
  /// \tparam InExpSet the expressionset type
  template <size_t Tape, typename InExpSet>
  auto
  project(const InExpSet& in_rs, const typename InExpSet::value_t& v)
    -> typename InExpSet::template project_t<Tape>::value_t
  {
    return project<Tape>(in_rs, in_rs.template project<Tape>(), v);
  }
}
