#pragma once

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

  /*------------------------.
  | tuple(expression...).   |
  `------------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename... ExpSets, size_t... I>
      expression
      tuple_(const std::vector<expression>& es,
             vcsn::detail::index_sequence<I...>)
      {
        auto rs = vcsn::tuple_expressionset(es[I]
                                            ->template as<ExpSets>()
                                            .expressionset()...);
        return make_expression(rs,
                               vcsn::tuple<decltype(rs), ExpSets...>
                               (rs,
                                es[I]
                                ->template as<ExpSets>()
                                .expression()...));
      }

      /// Bridge (tuple).
      template <typename... ExpSets>
      expression
      tuple_vector(const std::vector<expression>& es)
      {
        auto indices = vcsn::detail::make_index_sequence<sizeof...(ExpSets)>{};
        return tuple_<ExpSets...>(es, indices);
      }
    }
  }
}
