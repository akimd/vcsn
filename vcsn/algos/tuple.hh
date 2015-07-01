#pragma once

namespace vcsn
{
  /*---------------------------.
  | tuple(context, context).   |
  `---------------------------*/

  template <typename LhsLabelSet, typename LhsWeightSet,
            typename RhsLabelSet, typename RhsWeightSet>
  auto
  tuple(const context<LhsLabelSet, LhsWeightSet>& lhs,
        const context<RhsLabelSet, RhsWeightSet>& rhs)
    -> context<tupleset<LhsLabelSet, RhsLabelSet>,
               join_t<LhsWeightSet, RhsWeightSet>>
  {
    auto ls = make_tupleset(*lhs.labelset(), *rhs.labelset());
    auto ws = vcsn::join(*lhs.weightset(), *rhs.weightset());
    return {ls, ws};
  }


  /*---------------------------------.
  | tuple(expression, expression).   |
  `---------------------------------*/

  template <typename ValueSet, typename LhsValueSet, typename RhsValueSet>
  inline
  typename ValueSet::value_t
  tuple(const ValueSet& vs,
        const LhsValueSet&, const typename LhsValueSet::value_t& lv,
        const RhsValueSet&, const typename RhsValueSet::value_t& rv)
  {
    return vs.tuple(lv, rv);
  }

  /*---------------------------------------.
  | tuple(expressionset, expressionset).   |
  `---------------------------------------*/

  template <typename LhsExpSet, typename RhsExpSet>
  auto
  tuple(const LhsExpSet& lhs, const RhsExpSet& rhs)
    -> expressionset<decltype(tuple(lhs.context(), rhs.context()))>
  {
    auto ctx = tuple(lhs.context(), rhs.context());
    auto ids = join(lhs.identities(), rhs.identities());
    return {ctx, ids};
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (tuple).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      tuple_expression(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        const auto& lrs = l.expressionset();
        const auto& rrs = r.expressionset();
        auto rs = vcsn::tuple(lrs, rrs);
        return make_expression(rs,
                               ::vcsn::tuple(rs,
                                             lrs, l.expression(),
                                             rrs, r.expression()));
      }
    }
  }
}
