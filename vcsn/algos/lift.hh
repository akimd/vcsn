#pragma once

#include <map>

#include <boost/optional.hpp>

#include <vcsn/core/mutable-automaton.hh> // fresh_automaton_t_of
#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/labelset/oneset.hh>
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

    /// lift(ctx) -> ctx
    template <typename LabelSet, typename WeightSet>
    lifted_context_t<context<LabelSet, WeightSet>>
    lift_context(const context<LabelSet, WeightSet>& ctx,
                 vcsn::rat::identities ids = {})
    {
      auto rs_in = expressionset<context<LabelSet, WeightSet>>{ctx, ids};
      return {oneset{}, rs_in};
    }

    /// lift(expressionset) -> expressionset
    template <typename Context>
    lifted_expressionset_t<expressionset<Context>>
    lift_expressionset(const expressionset<Context>& rs,
                       boost::optional<vcsn::rat::identities> ids = {})
    {
      // FIXME: Boost 1.56 deprecates get_value_or in favor of value_or.
      return {lift_context(rs.context()), ids.get_value_or(rs.identities())};
    }
  }

  /*------------------------.
  | lift_tape(automaton).   |
  `------------------------*/

  namespace detail
  {

    template <std::size_t... I>
    using seq = vcsn::detail::index_sequence<I...>;

    /// Set of tapes
    template <typename S, typename L>
    struct tape_set;

    // Set of no tape is oneset
    template <typename LabelSet>
    struct tape_set<seq<>, LabelSet>
    {
      using type = oneset;
    };

    template <size_t... I, typename LabelSet>
    struct tape_set<seq<I...>, LabelSet>
    {
      using type = tupleset<typename LabelSet::template valueset_t<I>...>;
    };

    /// Helper structure for a lift of several tapes.
    template <typename Context, typename Tapes, typename Enable = void>
    struct lifted_context_tape_impl;

    /// Lift all the label tapes to the weights.
    template <typename Context>
    struct lifted_context_tape_impl<Context, vcsn::detail::index_sequence<>>
    {
      /// Result context.
      using context_t = lifted_context_t<Context>;
      using in_label_t = label_t_of<Context>;
      using label_t = label_t_of<context_t>;

      /// Conversion.
      static context_t value(const Context& ctx, vcsn::rat::identities ids)
      {
        return lift_context(ctx, ids);
      }

      /// Label in the output.
      static oneset::value_t
      kept_label(const in_label_t&)
      {
        return oneset::one();
      }

      /// Weight in the output.
      static in_label_t
      weight_label(const in_label_t& l)
      {
        return l;
      }
    };

    /// Specialization: lift only some tapes.
    template <typename... LabelSets, typename WeightSet,
              size_t... Tapes>
    struct lifted_context_tape_impl<context<tupleset<LabelSets...>, WeightSet>,
                                    vcsn::detail::index_sequence<Tapes...>,
                                    vcsn::enable_if_t<(0 < sizeof...(Tapes))>>
    {
      /// Input labelset
      using labelset_t = tupleset<LabelSets...>;
      using in_context_t = context<tupleset<LabelSets...>, WeightSet>;
      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      /// Index of all tapes
      using index_t
        = typename detail::make_index_sequence<labelset_t::size()>::type;

      using in_label_t = typename labelset_t::value_t;

      static constexpr size_t number_of_tapes = labelset_t::size();

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      /// Complement the list of indexes of tapes to be lifted, to get
      /// the list of tapes to be kept.
      using kept_index_t
        = sequence_difference<index_t, seq<Tapes...>>;

      /// Labelset of tapes to be kept.
      using kept_tapes_t = typename tape_set<kept_index_t, labelset_t>::type;

      /// List of indexes of tapes to be lifted.
      using weight_index_t = seq<Tapes...>;

      /// Labelset of tapes to be lifted.
      using weight_tapes_t
        = typename tape_set<weight_index_t, labelset_t>::type;

      using inner_context_t = context<weight_tapes_t, WeightSet>;

      /// Weightset of the result: lifted tapes plus former weightset.
      using expr_t = expressionset<inner_context_t>;

      /// Result context.
      using context_t = context<kept_tapes_t, expr_t>;

      /// Conversion.
      static context_t value(const in_context_t& ctx,
                             vcsn::rat::identities ids)
      {
        return value_(ctx, ids, weight_index_t{}, kept_index_t{});
      }

      template <size_t... WeightTapes, size_t... KeptTapes>
      static context_t value_(const in_context_t& ctx,
                              vcsn::rat::identities ids,
                              seq<WeightTapes...>,
                              seq<KeptTapes...>)
      {
        // The labelset.
        auto ls = kept_tapes_t{ctx.labelset()->template set<KeptTapes>()...};

        // The weightset.
        auto weight_tapes
          = weight_tapes_t{ctx.labelset()->template set<WeightTapes>()...};
        auto ictx = inner_context_t{weight_tapes, *ctx.weightset()};
        auto rs = expr_t{ictx, ids};

        return {ls, rs};
      }

      /// Label in the output.
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

      /// Weight in the output.
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
    using lifted_context_tape_t =
      lifted_context_tape_impl<Aut, seq<Tapes...>>;

    template <typename Aut, size_t... Tapes>
    using lifted_automaton_tape_t =
      mutable_automaton<typename lifted_context_tape_t<context_t_of<Aut>,
                                                       Tapes...>::context_t>;

    /// lift(ctx) -> ctx
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

    /// Lift some tapes of the transducer.
    ///
    /// \param a    the input automaton
    /// \param ids  the identities to use for the generated expressions
    template <typename Aut, size_t... Tapes>
    inline
    detail::lifted_automaton_tape_t<Aut, Tapes...>
    lift_tape(const Aut& a, vcsn::rat::identities ids = {})
    {
      using auto_in_t = Aut;
      using state_in_t = state_t_of<auto_in_t>;

      using lifter = detail::lifted_context_tape_t<context_t_of<Aut>, Tapes...>;
      auto ctx_out = lifter::value(a->context(), ids);

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

  /// Lift some label tapes to the weights.
  template <typename Aut, size_t... Tapes>
  inline
  detail::lifted_automaton_tape_t<Aut, Tapes...>
  lift(const Aut& a, vcsn::rat::identities ids = {})
  {
    return detail::lift_tape<Aut, Tapes...>(a, ids);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ids, typename... Tapes>
      automaton
      lift_automaton(const automaton& aut,
                     vcsn::rat::identities ids, integral_constant)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::lift<Aut, Tapes::value...>(a, ids));
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
