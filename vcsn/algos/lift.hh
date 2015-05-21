#pragma once

#include <map>

#include <vcsn/ctx/context.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/rat/expression.hh>

namespace vcsn
{

  namespace detail
  {
    /*--------------.
    | lift(types).  |
    `--------------*/

    template <typename Context>
    using lifted_context_t =
      context<oneset, expressionset<Context>>;

    template <typename Aut>
    using lifted_automaton_t =
      mutable_automaton<lifted_context_t<context_t_of<Aut>>>;

    template <typename ExpSet>
    using lifted_expressionset_t =
      expressionset<lifted_context_t<context_t_of<ExpSet>>>;

    // lift(ctx) -> ctx
    template <typename LabelSet, typename WeightSet>
    lifted_context_t<context<LabelSet, WeightSet>>
    lift_context(const context<LabelSet, WeightSet>& ctx)
    {
      auto rs_in
        = expressionset<context<LabelSet, WeightSet>>(ctx,
                                                  rat::identities::trivial);
      return {oneset{}, rs_in};
    }

    // lift(expressionset) -> expressionset
    template <typename Context>
    lifted_expressionset_t<expressionset<Context>>
    lift_expressionset(const expressionset<Context>& rs)
    {
      return {lift_context(rs.context()), rs.identities()};
    }

  }

  /*------------------.
  | lift(automaton).  |
  `------------------*/

  template <typename Aut>
  inline
  detail::lifted_automaton_t<Aut>
  lift(const Aut& a)
  {
    using auto_in_t = typename Aut::element_type;
    using ctx_in_t = context_t_of<auto_in_t>;
    using state_in_t = state_t_of<auto_in_t>;

    // Produce expressions of the same context as the original automaton.
    using rs_in_t = expressionset<ctx_in_t>;
    rs_in_t rs_in{a->context(), rs_in_t::identities_t::trivial};

    auto ctx_out = detail::lift_context(a->context());
    using auto_out_t = detail::lifted_automaton_t<auto_in_t>;
    using state_out_t = state_t_of<auto_out_t>;
    auto_out_t res = make_shared_ptr<auto_out_t>(ctx_out);
    std::map<state_in_t, state_out_t> map;
    map[a->pre()] = res->pre();
    map[a->post()] = res->post();
    for (auto s: a->states())
      map[s] = res->new_state();

    for (auto t: a->all_transitions())
      if (a->src_of(t) == a->pre())
        res->add_initial(map[a->dst_of(t)],
                        rs_in.lmul(a->weight_of(t), rs_in.one()));
      else if (a->dst_of(t) == a->post())
        res->add_final(map[a->src_of(t)],
                      rs_in.lmul(a->weight_of(t), rs_in.one()));
      else
        res->add_transition
          (map[a->src_of(t)], map[a->dst_of(t)],
           {},
           rs_in.lmul(a->weight_of(t), rs_in.atom(a->label_of(t))));
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (lift).
      template <typename Aut>
      automaton
      lift_automaton(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::lift(a));
      }
    }
  }


  /*--------------------.
  | lift(expression).   |
  `--------------------*/

  namespace detail
  {
    template <typename Exp>
    using lifted_expression_t =
      typename lifted_context_t<context_t_of<Exp>>::expression_t;
  }

  template <typename ExpSet>
  inline
  typename detail::lifted_expressionset_t<ExpSet>::value_t
  lift(const ExpSet& rs, const typename ExpSet::value_t& e)
  {
    auto lrs = detail::lift_expressionset(rs);
    return lrs.lmul(e, lrs.one());
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (lift).
      template <typename ExpSet>
      expression
      lift_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& es = e.expressionset();
        return make_expression(::vcsn::detail::lift_expressionset(es),
                               ::vcsn::lift(es, e.expression()));
      }
    }
  }
} // vcsn::
