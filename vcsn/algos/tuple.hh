#pragma once

#include <vcsn/core/rat/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/labelset/tupleset.hh>

namespace vcsn
{
  /*-----------------------------.
  | tuple_context(context...).   |
  `-----------------------------*/

  template <typename... Ctx>
  auto
  tuple_context(Ctx&&... ctx)
    -> context<tupleset<labelset_t_of<Ctx>...>,
               join_t<weightset_t_of<Ctx>...>>
  {
    auto ls = make_tupleset(*ctx.labelset()...);
    auto ws = join(*ctx.weightset()...);
    return {ls, ws};
  }


  /*---------------------------------------.
  | tuple_expansionset(expansionset...).   |
  `---------------------------------------*/

  template <typename... ExpansionSets>
  auto
  tuple_expansionset(const ExpansionSets&... ess)
    -> rat::expansionset<decltype(tuple_expressionset(ess.expressionset()...))>
  {
    return {tuple_expressionset(ess.expressionset()...)};
  }

  /*------------------------.
  | tuple(expression...).   |
  `------------------------*/

  template <typename ValueSet, typename... ValueSets>
  inline
  typename ValueSet::value_t
  tuple(const ValueSet& vs,
        const typename ValueSets::value_t&... v)
  {
    return vs.tuple(v...);
  }

  /*-----------------------------------------.
  | tuple_expressionset(expressionset...).   |
  `-----------------------------------------*/

  template <typename... ExpSets>
  auto
  tuple_expressionset(const ExpSets&... rss)
    -> expressionset<decltype(tuple_context(rss.context()...))>
  {
    auto ctx = tuple_context(rss.context()...);
    auto ids = join(rss.identities()...);
    return {ctx, ids};
  }

  /*-----------------------.
  | tuple(expansion...).   |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge helper.
      template <typename ExpansionSets, size_t... I>
      expansion
      tuple_(const std::vector<expansion>& es,
             vcsn::detail::index_sequence<I...>)
      {
        auto xs
          = vcsn::tuple_expansionset
          (es[I]
           ->template as<tuple_element_t<I, ExpansionSets>>()
           .valueset()...);
        return
          {xs,
           vcsn::tuple<decltype(xs), tuple_element_t<I, ExpansionSets>...>
           (xs,
            es[I]->template as<tuple_element_t<I, ExpansionSets>>()
            .value()...)};
      }

      /// Bridge (tuple).
      template <typename ExpansionSets>
      expansion
      tuple_expansion(const std::vector<expansion>& es)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<ExpansionSets>{}>{};
        return tuple_<ExpansionSets>(es, indices);
      }
    }
  }

  /*------------------------.
  | tuple(expression...).   |
  `------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge helper.
      template <typename ExpSets, size_t... I>
      expression
      tuple_(const std::vector<expression>& es,
             vcsn::detail::index_sequence<I...>)
      {
        auto rs
          = vcsn::tuple_expressionset
          (es[I]->template as<tuple_element_t<I, ExpSets>>().valueset()...);
        return
          {rs,
           vcsn::tuple<decltype(rs), tuple_element_t<I, ExpSets>...>
           (rs,
            es[I]
            ->template as<tuple_element_t<I, ExpSets>>().value()...)};
      }

      /// Bridge (tuple).
      template <typename ExpSets>
      expression
      tuple_expression(const std::vector<expression>& es)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<ExpSets>{}>{};
        return tuple_<ExpSets>(es, indices);
      }
    }
  }


  /*-----------------------------------------.
  | tuple_polynomialset(polynomialset...).   |
  `-----------------------------------------*/

  template <typename... PolynomialSets>
  auto
  tuple_polynomialset(const PolynomialSets&... pss)
    -> polynomialset<decltype(tuple_context(pss.context()...))>
  {
    auto ctx = tuple_context(pss.context()...);
    return {ctx};
  }


  /*------------------------.
  | tuple(polynomial...).   |
  `------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge helper.
      template <typename PolynomialSet, size_t... I>
      polynomial
      tuple_(const std::vector<polynomial>& polys,
             vcsn::detail::index_sequence<I...>)
      {
        auto ps
          = vcsn::tuple_polynomialset
          (polys[I]->template as<tuple_element_t<I, PolynomialSet>>().valueset()...);
        return
          {ps,
           vcsn::tuple<decltype(ps), tuple_element_t<I, PolynomialSet>...>
           (ps,
            polys[I]
            ->template as<tuple_element_t<I, PolynomialSet>>().value()...)};
      }

      /// Bridge (tuple).
      template <typename PolynomialSets>
      polynomial
      tuple_polynomial(const std::vector<polynomial>& ps)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<PolynomialSets>{}>{};
        return tuple_<PolynomialSets>(ps, indices);
      }
    }
  }
}
