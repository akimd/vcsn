#pragma once

#include <vcsn/algos/focus.hh>
#include <vcsn/algos/insplit.hh>
#include <vcsn/core/transition-map.hh>
#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/tuple.hh> // make_index_sequence, cross_tuple
#include <vcsn/misc/zip-maps.hh>

namespace vcsn
{
  namespace detail
  {
    /*---------------------------------.
    | composer<automaton, automaton>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) composition.
    template <typename Lhs, typename Rhs>
    class composer
    {
      static_assert(Lhs::element_type::full_context_t::is_lat,
                    "compose: lhs labelset must be a tupleset");
      static_assert(Rhs::element_type::full_context_t::is_lat,
                    "compose: rhs labelset must be a tupleset");

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

    public:
      using clhs_t = Lhs;
      using crhs_t = Rhs;
      // clhs_t and crhs_t are permutation automata, yet we need to
      // read the res_label_t from their wrapped automaton type.
      using hidden_l_labelset_t = typename clhs_t::element_type::res_labelset_t;
      using hidden_r_labelset_t = typename crhs_t::element_type::res_labelset_t;
      using hidden_l_label_t = typename hidden_l_labelset_t::value_t;
      using hidden_r_label_t = typename hidden_r_labelset_t::value_t;

      static_assert(std::is_same<labelset_t_of<clhs_t>,
                                 labelset_t_of<crhs_t>>::value,
                    "compose: common tape must be of same type");

      using middle_labelset_t = labelset_t_of<clhs_t>;
      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltration.
      using labelset_t = typename concat_tupleset<hidden_l_labelset_t,
                                                  hidden_r_labelset_t>::type;
      using weightset_t = join_t<weightset_t_of<context_t_of<Lhs>>,
                                 weightset_t_of<context_t_of<Rhs>>>;

      using res_label_t = typename labelset_t::value_t;
      using context_t = ::vcsn::context<labelset_t, weightset_t>;

      /// The type of the resulting automaton.
      using automaton_t = tuple_automaton<mutable_automaton<context_t>,
                                          Lhs, Rhs>;

      /// Result state type.
      using state_t = state_t_of<automaton_t>;
      /// Tuple of states of input automata.
      using state_name_t = typename automaton_t::element_type::state_name_t;

      composer(const Lhs& lhs, const Rhs& rhs)
        : res_(make_shared_ptr<automaton_t>(make_mutable_automaton(make_context_(lhs, rhs)),
                                            lhs, rhs))
        , transition_maps_{{lhs, *res_->weightset()},
                           {rhs, *res_->weightset()}}
      {}

      /// The (accessible part of the) product of \a lhs_ and \a rhs_.
      automaton_t operator()()
      {
        initialize_compose();

        while (!res_->todo_.empty())
          {
            const auto& p = res_->todo_.front();
            add_compose_transitions(std::get<1>(p), std::get<0>(p));
            res_->todo_.pop_front();
          }
        return std::move(res_);
      }

    private:
      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <typename A>
      using transition_map_t
        = transition_map<A, weightset_t, false, true, true>;

      static labelset_t make_labelset_(const hidden_l_labelset_t& ll,
                                       const hidden_r_labelset_t& rl)
      {
        return make_labelset_(ll, make_index_sequence<hidden_l_labelset_t::size()>{},
                              rl, make_index_sequence<hidden_r_labelset_t::size()>{});
      }

      template <std::size_t... I1, std::size_t... I2>
      static labelset_t make_labelset_(const hidden_l_labelset_t& ll,
                                       seq<I1...>,
                                       const hidden_r_labelset_t& rl,
                                       seq<I2...>)
      {
        return labelset_t{std::get<I1>(ll.sets())...,
                          std::get<I2>(rl.sets())...};
      }

      static context_t
      make_context_(const Lhs& lhs, const Rhs& rhs)
      {
        return {make_labelset_(lhs->res_labelset(), rhs->res_labelset()),
                join(*lhs->weightset(), *rhs->weightset())};
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_compose()
      {
        res_->todo_.emplace_back(res_->pre_(), res_->pre());
      }

      res_label_t join_label(const hidden_l_label_t& ll,
                             const hidden_r_label_t& rl)
      {
        return std::tuple_cat(ll, rl);
      }

      template<typename Aut>
      vcsn::enable_if_t<labelset_t_of<Aut>::has_one(),
                        typename Aut::element_type::res_label_t>
      get_hidden_one(const Aut& aut)
      {
        return aut->hidden_one();
      }

      template<typename Aut>
      vcsn::enable_if_t<!labelset_t_of<Aut>::has_one(),
                        typename Aut::element_type::res_label_t>
      get_hidden_one(const Aut&)
      {
        raise("should not get here");
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_compose_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        const auto& lhs = std::get<0>(res_->auts_);
        const auto& rhs = std::get<1>(res_->auts_);

        // Outgoing transition cache.
        const auto& ltm = std::get<0>(transition_maps_)[std::get<0>(psrc)];
        const auto& rtm = std::get<1>(transition_maps_)[std::get<1>(psrc)];

        bool has_eps_out = false;
        if (!is_spontaneous_in(rhs, std::get<1>(psrc))
            // "Loop" only on the spontaneous transitions.  "One" is
            // guaranteed to be first in the transition maps.
            && !ltm.empty()
            && lhs->labelset()->is_one(ltm.begin()->first))
          {
            has_eps_out = true;
            for (auto t: ltm.begin()->second)
              res_->new_transition(src,
                                   res_->state(t.dst, std::get<1>(psrc)),
                                   join_label(lhs->hidden_label_of(t.transition),
                                              get_hidden_one(rhs)),
                                   t.weight());
          }

        // If lhs did not issue spontaneous transitions but has proper
        // transitions, issue follow all the rhs spontaneous
        // transitions.
        const bool lhs_has_proper_trans =
          !ltm.empty()
          && (!lhs->labelset()->is_one(ltm.begin()->first)
              || 2 <= ltm.size());

        if ((!has_eps_out || lhs_has_proper_trans)
            && !rtm.empty()
            && rhs->labelset()->is_one(rtm.begin()->first))
          for (auto t: rtm.begin()->second)
            res_->new_transition(src,
                                 res_->state(std::get<0>(psrc), t.dst),
                                 join_label(get_hidden_one(lhs),
                                            rhs->hidden_label_of(t.transition)),
                                 t.weight());

        for (auto t: zip_maps(ltm, rtm))
          // The type of the common label is that of the visible tape
          // of either automata.
          if (!lhs->labelset()->is_one(t.first))
            // These are always new transitions: first because the
            // source state is visited for the first time, and second
            // because the couple (left destination, label) is unique,
            // and so is (right destination, label).
            cross_tuple
              ([&] (const typename transition_map_t<Lhs>::transition& lts,
                    const typename transition_map_t<Rhs>::transition& rts)
               {
                 res_->new_transition
                   (src,
                    res_->state(lts.dst, rts.dst),
                    join_label(lhs->hidden_label_of(lts.transition),
                               rhs->hidden_label_of(rts.transition)),
                    res_->weightset()->mul(lts.weight(), rts.weight()));
               },
               t.second);
      }

      template <typename A>
      vcsn::enable_if_t<labelset_t_of<A>::has_one(), bool>
      is_one(const A& aut, transition_t_of<A> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      template <typename A>
      constexpr
      vcsn::enable_if_t<!labelset_t_of<A>::has_one(), bool>
      is_one(const A&, transition_t_of<A>)
      const
      {
        return false;
      }

      /// Check if the state has only incoming spontaneous
      /// transitions.  As it is in the case of the one-free labelset,
      /// it's always false.
      template <typename Aut>
      constexpr
      vcsn::enable_if_t<!labelset_t_of<Aut>::has_one(), bool>
      is_spontaneous_in(const Aut&, state_t_of<Aut>) const
      {
        return false;
      }

      /// Whether the state has only incoming spontaneous transitions.
      /// The automaton has been insplit, so either all incoming transitions
      /// are proper, or all transitions are spontaneous (including the first
      /// one).
      template <typename Aut>
      vcsn::enable_if_t<labelset_t_of<Aut>::has_one(), bool>
      is_spontaneous_in(const Aut& rhs, state_t_of<Aut> rst) const
      {
        auto rin = rhs->all_in(rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr);
      }

      /// The computed product.
      automaton_t res_;
      /// Transition caches.
      std::tuple<transition_map_t<Lhs>, transition_map_t<Rhs>> transition_maps_;
    };

    template <typename Lhs, typename Rhs>
    auto
    make_composer(Lhs& lhs, Rhs& rhs)
      -> typename detail::composer<Lhs, Rhs>
    {
      return detail::composer<Lhs, Rhs>{lhs, rhs};
    }
  }

  /*--------------------------------.
  | compose(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) composition.
  template <typename Lhs, typename Rhs,
            std::size_t OutTape = 1, std::size_t InTape = 0>
  auto
  compose(Lhs& lhs, Rhs& rhs)
    -> typename detail::composer<focus_automaton<OutTape, Lhs>,
                                 focus_automaton<InTape, Rhs>>::automaton_t
  {
    auto l = focus<OutTape>(lhs);
    auto r = insplit(focus<InTape>(rhs));
    auto compose = detail::make_composer(l, r);
    return compose();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      compose(automaton& lhs, automaton& rhs)
      {
        auto& l = lhs->as<Lhs>();
        auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::compose(l, r));
      }
    }
  }
}
