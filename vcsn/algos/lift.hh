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
    /*---------.
    | lifter.  |
    `---------*/

    /// Helper structure for a lift of several tapes.
    ///
    /// \tparam Context  the context to lift.
    /// \tparam Tapes    an index_sequence of tape numbers.
    /// \tparam Enable   used to enable/disable specializations using enable_if.
    template <typename Context, typename Tapes, typename Enable = void>
    struct lifter_impl;

    /// Lift all the label tapes to the weights.
    template <typename Context>
    struct lifter_impl<Context, vcsn::detail::index_sequence<>>
    {
      /// Input context.
      using in_context_t = Context;
      using in_label_t = label_t_of<in_context_t>;

      /// Result.
      /// Output weightset: the expressionset based on the input context.
      using weightset_t = expressionset<in_context_t>;
      using context_t = context<oneset, weightset_t>;

      /// Lift a context.
      static context_t value(const in_context_t& ctx, vcsn::rat::identities ids)
      {
        return {oneset{}, weightset_t{ctx, ids}};
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
    struct lifter_impl<context<tupleset<LabelSets...>, WeightSet>,
                       vcsn::detail::index_sequence<Tapes...>,
                       std::enable_if_t<(0 < sizeof...(Tapes))>>
    {
      /// Input labelset.
      using in_labelset_t = tupleset<LabelSets...>;
      using in_context_t = context<in_labelset_t, WeightSet>;
      using in_label_t = typename in_labelset_t::value_t;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = detail::index_sequence<I...>;

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

      /// Index of all input tapes.
      using index_t
        = typename detail::make_index_sequence<in_labelset_t::size()>::type;

      static constexpr size_t number_of_tapes = in_labelset_t::size();

      template <size_t I>
      using tape_labelset_t = typename in_labelset_t::template valueset_t<I>;

      /// Complement the list of indexes of tapes to be lifted, to get
      /// the list of tapes to be kept.
      using kept_index_t
        = sequence_difference<index_t, seq<Tapes...>>;

      /// Labelset of tapes to be kept.
      using labelset_t = typename tape_set<kept_index_t, in_labelset_t>::type;

      /// List of indexes of tapes to be lifted.
      using weight_index_t = seq<Tapes...>;

      /// Labelset of tapes to be lifted.
      using weight_tapes_t
        = typename tape_set<weight_index_t, in_labelset_t>::type;

      using inner_context_t = context<weight_tapes_t, WeightSet>;

      /// Weightset of the result: lifted tapes plus former weightset.
      using weightset_t = expressionset<inner_context_t>;

      /// Result context.
      using context_t = context<labelset_t, weightset_t>;

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
        auto ls = labelset_t{ctx.labelset()->template set<KeptTapes>()...};

        // The weightset.
        auto weight_tapes
          = weight_tapes_t{ctx.labelset()->template set<WeightTapes>()...};
        auto ictx = inner_context_t{weight_tapes, *ctx.weightset()};
        auto rs = weightset_t{ictx, ids};

        return {ls, rs};
      }

      /// Label in the output.
      static typename labelset_t::value_t
      kept_label(const in_label_t& l)
      {
        return kept_label_(l, kept_index_t{});
      }

      template <size_t... I>
      static typename labelset_t::value_t
      kept_label_(const in_label_t& l, seq<I...>)
      {
        return typename labelset_t::value_t{std::get<I>(l)...};
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

    /// A traits to lift the Tapes of the labels to the weights.
    template <typename Ctx, size_t... Tapes>
    using lifter_t = lifter_impl<Ctx, detail::index_sequence<Tapes...>>;

    template <Automaton Aut, size_t... Tapes>
    using lifted_automaton_t =
      mutable_automaton<typename lifter_t<context_t_of<Aut>,
                                          Tapes...>::context_t>;

    /// lift(ctx) -> ctx
    template <typename LabelSet, typename WeightSet, size_t... Tapes>
    typename lifter_t<context<LabelSet, WeightSet>, Tapes...>::context_t
    lift_context(const context<LabelSet, WeightSet>& ctx)
    {
      return (lifter_t<context<LabelSet, WeightSet>, Tapes...>
              ::value(ctx, {}));
    }
  }

  /*-------------------.
  | lift(automaton).   |
  `-------------------*/

  /// Lift some tapes of the transducer.
  ///
  /// \param a    the input automaton
  /// \param ids  the identities to use for the generated expressions
  template <Automaton Aut, size_t... Tapes>
  inline
  detail::lifted_automaton_t<Aut, Tapes...>
  lift(const Aut& a, vcsn::rat::identities ids = {})
  {
    using auto_in_t = Aut;
    using state_in_t = state_t_of<auto_in_t>;

    using lifter = detail::lifter_t<context_t_of<Aut>, Tapes...>;
    auto ctx_out = lifter::value(a->context(), ids);

    // ExpressionSet
    const auto& rs_in = *ctx_out.weightset();

    using auto_out_t = detail::lifted_automaton_t<auto_in_t, Tapes...>;
    using state_out_t = state_t_of<auto_out_t>;
    auto_out_t res = make_shared_ptr<auto_out_t>(ctx_out);

    // FIXME: a sufficiently generic vcsn::copy should suffice.
    auto map = std::map<state_in_t, state_out_t>{};
    map[a->pre()] = res->pre();
    map[a->post()] = res->post();
    for (auto s: a->states())
      map[s] = res->new_state();

    for (auto t: all_transitions(a))
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

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Ids, typename... Tapes>
      automaton
      lift_automaton(const automaton& aut,
                     vcsn::rat::identities ids, integral_constant)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::lift<Aut, Tapes::value...>(a, ids);
      }
    }
  }

  /*--------------------.
  | lift(expression).   |
  `--------------------*/

  namespace detail
  {
    template <typename ExpSet>
    using lifted_expressionset_t =
      expressionset<typename lifter_t<context_t_of<ExpSet>>::context_t>;

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

  /// Move all the labels to the weights.
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
