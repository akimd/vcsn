#pragma once

#include <map>

#include <vcsn/ctx/context.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/rat/expression.hh>
#include <vcsn/misc/name.hh>

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
      auto rs_in = expressionset<context<LabelSet, WeightSet>>{ctx};
      return {oneset{}, rs_in};
    }

    // lift(expressionset) -> expressionset
    template <typename Context>
    lifted_expressionset_t<expressionset<Context>>
    lift_expressionset(const expressionset<Context>& rs)
    {
      return {lift_context(rs.context()), rs.identities()};
    }





    // Helper structure for a lift of several tapes
    template<typename Context, size_t... Tapes>
    struct lifted_context_tape_t;

    template <typename... LabelSets, typename WeightSet, size_t... Tapes>
    struct lifted_context_tape_t<context<tupleset<LabelSets...>, WeightSet>, Tapes...>
    {
      using labelset_t = tupleset<LabelSets...>;
      using in_context_t = context<tupleset<LabelSets...>, WeightSet>;
      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;
      using index_t = detail::make_index_sequence<labelset_t::size()>;

      using in_label_t = typename labelset_t::value_t;

      static constexpr size_t number_of_tapes = labelset_t::size();

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      // List of indexes of tapes to be lifted
      using weight_index_t = seq<Tapes...>;

      // Complement the list of indexes of tapes to be lifted, to get the list
      // of tapes to be kept
      using kept_index_t = sequence_difference<index_t, seq<Tapes...>>;

      template <typename S>
      struct kept_taper {};

      template <size_t... I>
      struct kept_taper<seq<I...>>
      {
        using type = tupleset<tape_labelset_t<I>...>;
      };

      // Labelset of tapes to be kept
      using kept_tapes_t = typename kept_taper<kept_index_t>::type;

      // Labelset of tapes to be lifted
      using weight_tapes_t = tupleset<tape_labelset_t<Tapes>...>;

      using inner_context_t = context<weight_tapes_t, WeightSet>;

      // Weightset of the result : lifted tapes plus former weightset
      using expr_t = expressionset<inner_context_t>;

      // Result context
      using context_t = context<kept_tapes_t, expr_t>;

      // conversion
      static context_t value(const in_context_t& ctx)
      {
        return value_(ctx, kept_index_t{});
      }

      template <size_t... KeptTapes>
      static context_t value_(const in_context_t& ctx, seq<KeptTapes...>)
      {
        auto rs =
          expr_t{inner_context_t{weight_tapes_t{ctx.labelset()->template set<Tapes>()...},
                                 *ctx.weightset()}};
        return {kept_tapes_t{ctx.labelset()->template set<KeptTapes>()...}, rs};
      }

      static typename kept_tapes_t::value_t
      kept_label(const in_label_t& l)
      {
        return kept_label_(l, kept_index_t{});
      }

      template <size_t... I>
      static typename kept_tapes_t::value_t
      kept_label_(const in_label_t& l, seq<I...>)
      {
        return typename kept_tapes_t::value_t{std::get<I>(l)...};
      }

      static typename weight_tapes_t::value_t
      weight_label(const in_label_t& l)
      {
        return weight_label_(l, weight_index_t{});
      }

      template <size_t... I>
      static typename weight_tapes_t::value_t
      weight_label_(const in_label_t& l, seq<I...>)
      {
        return typename weight_tapes_t::value_t{std::get<I>(l)...};
      }
    };

    template <typename Aut, size_t... Tapes>
    using lifted_automaton_tape_t =
      mutable_automaton<typename lifted_context_tape_t<context_t_of<Aut>, Tapes...>::context_t>;

    // lift(ctx) -> ctx
    template <typename LabelSet, typename WeightSet, size_t... Tapes>
    lifted_context_tape_t<context<LabelSet, WeightSet>, Tapes...>
    lift_context_tape(const context<LabelSet, WeightSet>& ctx)
    {
      using ctx_t =
        lifted_context_tape_t<context<LabelSet, WeightSet>, Tapes...>;
      auto rs_in =
        expressionset<context<typename ctx_t::weight_tapes_t, WeightSet>>{ctx};
      return {oneset{}, rs_in};
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
    auto rs_in = rs_in_t{a->context()};

    auto ctx_out = detail::lift_context(a->context());
    using auto_out_t = detail::lifted_automaton_t<auto_in_t>;
    using state_out_t = state_t_of<auto_out_t>;
    auto_out_t res = make_shared_ptr<auto_out_t>(ctx_out);
    auto map = std::map<state_in_t, state_out_t>{};
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

  namespace detail
  {
    // Lift only certain tapes of the transducer
    template <typename Aut, size_t... Tapes>
    inline
    detail::lifted_automaton_tape_t<Aut, Tapes...>
    lift_tape(const Aut& a)
    {
      using auto_in_t = typename Aut::element_type;
      using state_in_t = state_t_of<auto_in_t>;

      using lifter = detail::lifted_context_tape_t<context_t_of<Aut>, Tapes...>;
      auto ctx_out = lifter::value(a->context());

      // ExpressionSet
      const auto& rs_in = *ctx_out.weightset();

      using auto_out_t = detail::lifted_automaton_tape_t<auto_in_t, Tapes...>;
      using state_out_t = state_t_of<auto_out_t>;
      auto_out_t res = make_shared_ptr<auto_out_t>(ctx_out);
      auto map = std::map<state_in_t, state_out_t>{};
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
             lifter::kept_label(a->label_of(t)),
             rs_in.lmul(a->weight_of(t),
                        rs_in.atom(lifter::weight_label(a->label_of(t)))));
      return res;
    }
  }

  template <typename Aut, size_t FirstTape, size_t... Tapes>
  inline
  detail::lifted_automaton_tape_t<Aut, FirstTape, Tapes...>
  lift(const Aut& a)
  {
    return detail::lift_tape<Aut, FirstTape, Tapes...>(a);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename... Tapes>
      automaton
      lift_automaton_tape(const automaton& aut, integral_constant)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::lift<Aut, Tapes::value...>(a));
      }
    }
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
