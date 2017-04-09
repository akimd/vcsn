#pragma once

#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/tuple.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  /*--------------------.
  | compose_labelset.   |
  `--------------------*/

  namespace detail
  {
    template <typename... LS1, typename... LS2,
              std::size_t... I1, std::size_t... I2>
    auto
    compose_labelset_impl(const tupleset<LS1...>& ls1,
                          const tupleset<LS2...>& ls2,
                          detail::index_sequence<I1...>,
                          detail::index_sequence<I2...>)
    {
      return make_tupleset(ls1.template set<I1>()...,
                           ls2.template set<I2>()...);
    }
  }

  template <typename... LS1, typename... LS2>
  auto
  compose_labelset(const tupleset<LS1...>& ls1,
                   const tupleset<LS2...>& ls2)
  {
    // Tape of the lhs on which we compose.
    constexpr auto out = tupleset<LS1...>::size() - 1;
    // Tape of the rhs on which we compose.
    constexpr auto in = 0;
    using indices1_t = detail::punched_sequence<tupleset<LS1...>::size(), out>;
    using indices2_t = detail::punched_sequence<tupleset<LS2...>::size(), in>;
    return detail::compose_labelset_impl(ls1, ls2,
                                         indices1_t{}, indices2_t{});
  }


  /*-------------------------------.
  | compose_context(context...).   |
  `-------------------------------*/

  template <typename Ctx1, typename Ctx2>
  auto
  compose_context(const Ctx1& ctx1, const Ctx2& ctx2)
  {
    auto ls = compose_labelset(*ctx1.labelset(), *ctx2.labelset());
    auto ws = join(*ctx1.weightset(), *ctx2.weightset());
    return make_context(ls, ws);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename ContextLhs, typename ContextRhs>
      context
      compose_context(const context& lhs, const context& rhs)
      {
        return ::vcsn::compose_context(lhs.as<ContextLhs>(),
                                       rhs.as<ContextRhs>());
      }
    }
  }


  /*-------------------------.
  | compose(Value, Value).   |
  `-------------------------*/

  /// Composition of values.
  template <typename ValueSet>
  typename ValueSet::value_t
  compose(const ValueSet& vs,
          const typename ValueSet::value_t& lhs,
          const typename ValueSet::value_t& rhs)
  {
    return vs.compose(lhs, rhs);
  }


  /*---------------------------------.
  | compose(expansion, expansion).   |
  `---------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expansion
      compose_expansion(const expansion& lhs, const expansion& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::compose(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }

  /*-----------------------------------.
  | compose(expression, expression).   |
  `-----------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      compose_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::compose(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }

  /*-------------------------.
  | compose(label, label).   |
  `-------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename LabelSetLhs, typename LabelSetRhs>
      label
      compose_label(const label& lhs, const label& rhs)
      {
        const auto& l1 = lhs->as<LabelSetLhs>();
        const auto& l2 = rhs->as<LabelSetRhs>();
        auto ls = compose_labelset(l1.valueset(), l2.valueset());
        return {ls,
                ls.compose(l1.valueset(), l1.value(),
                           l2.valueset(), l2.value())};
      }
    }
  }

  /*-----------------------------------.
  | compose(polynomial, polynomial).   |
  `-----------------------------------*/

  template <typename Ctx1, typename Ctx2>
  auto
  compose_polynomialset(const polynomialset<Ctx1>& ps1,
                        const polynomialset<Ctx2>& ps2)
  {
    return make_polynomialset(compose_context(ps1.context(),
                                              ps2.context()));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compose).
      template <typename PolSetLhs, typename PolSetRhs>
      polynomial
      compose_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        const auto& p1 = lhs->as<PolSetLhs>();
        const auto& p2 = rhs->as<PolSetRhs>();
        auto ps = compose_polynomialset(p1.valueset(), p2.valueset());
        return {ps,
                ps.compose(p1.valueset(), p1.value(),
                           p2.valueset(), p2.value())};
      }
    }
  }
}
