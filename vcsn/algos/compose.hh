#pragma once

#include <vcsn/algos/focus.hh>
#include <vcsn/algos/insplit.hh>
#include <vcsn/core/lazy-tuple-automaton.hh>
#include <vcsn/core/transition-map.hh>
#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/tuple.hh> // make_index_sequence, cross_tuple
#include <vcsn/misc/zip-maps.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  namespace detail
  {
#define DEFINE(Type)                                                      \
    template <Automaton Aut>                                              \
    struct Type ## _of_impl                                               \
    {                                                                     \
      using type = typename Aut::element_type::Type;                      \
    };                                                                    \
                                                                          \
    template <Automaton Aut>                                              \
    struct Type ## _of_impl<insplit_automaton<Aut>>                       \
      : Type ## _of_impl<Aut>                                             \
    {};                                                                   \
                                                                          \
    template <Automaton Aut>                                              \
    using Type ## _of                                                     \
      = typename Type ## _of_impl<Aut>::type

    DEFINE(res_labelset_t);
    DEFINE(res_label_t);
    DEFINE(full_context_t);

#undef DEFINE

    /// Build the (accessible part of the) composition.
    template <Automaton Lhs, Automaton Rhs>
    struct composed_type
    {
      using clhs_t = Lhs;
      using crhs_t = Rhs;
      // clhs_t and crhs_t are permutation automata, yet we need to
      // read the res_label_t from their wrapped automaton type.
      using hidden_l_labelset_t = res_labelset_t_of<clhs_t>;
      using hidden_r_labelset_t = res_labelset_t_of<crhs_t>;
      using hidden_l_label_t = typename hidden_l_labelset_t::value_t;
      using hidden_r_label_t = typename hidden_r_labelset_t::value_t;


      /// The type of context of the result.
      using labelset_t = typename concat_tupleset<hidden_l_labelset_t,
                                                  hidden_r_labelset_t>::type;
      using weightset_t = join_t<weightset_t_of<context_t_of<Lhs>>,
                                 weightset_t_of<context_t_of<Rhs>>>;

      using res_label_t = typename labelset_t::value_t;
      using context_t = ::vcsn::context<labelset_t, weightset_t>;

      using out_t = mutable_automaton<context_t>;
    };


    /*------------------------------------------.
    | compose_automaton<automaton, automaton>.  |
    `------------------------------------------*/

    /// Build the (accessible part of the) composition.
    template <bool Lazy, Automaton Lhs, Automaton Rhs>
    class compose_automaton_impl
      : public lazy_tuple_automaton<compose_automaton_impl<Lazy, Lhs, Rhs>,
                                    true,
                                    Lazy,
                                    typename composed_type<Lhs, Rhs>::out_t,
                                    Lhs, Rhs>
    {
      static_assert(full_context_t_of<Lhs>::is_lat,
                    "compose: lhs labelset must be a tupleset");
      static_assert(full_context_t_of<Rhs>::is_lat,
                    "compose: rhs labelset must be a tupleset");

      static_assert(std::is_same<labelset_t_of<Lhs>,
                                 labelset_t_of<Rhs>>::value,
                    "compose: common tape must be of same type");

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

    public:

      using self_t = compose_automaton_impl;

      using type_helper_t = composed_type<Lhs, Rhs>;
      using hidden_l_labelset_t = typename type_helper_t::hidden_l_labelset_t;
      using hidden_r_labelset_t = typename type_helper_t::hidden_r_labelset_t;
      using hidden_l_label_t = typename type_helper_t::hidden_l_label_t;
      using hidden_r_label_t = typename type_helper_t::hidden_r_label_t;


      /// The type of context of the result.
      using labelset_t = typename type_helper_t::labelset_t;
      using weightset_t = typename type_helper_t::weightset_t;

      using res_label_t = typename type_helper_t::res_label_t;
      using context_t = typename type_helper_t::context_t;

      using out_t = typename type_helper_t::out_t;
      using super_t = lazy_tuple_automaton<self_t, true, Lazy, out_t, Lhs, Rhs>;
      /// Result state type.
      using state_t = typename super_t::state_t;
      /// Tuple of states of input automata.
      using state_name_t = typename super_t::state_name_t;

      using super_t::aut_;

      static symbol sname()
      {
        static symbol res(std::string{"compose_automaton<"}
                          + (Lazy ? "true" : "false") + ", "
                          + Lhs::element_type::sname() + ", "
                          + Rhs::element_type::sname() + ">");
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "compose_automaton<";
        std::get<0>(aut_->auts_)->print_set(o, fmt) << ", ";
        return std::get<1>(aut_->auts_)->print_set(o, fmt) << ">";
      }

      compose_automaton_impl(const Lhs& lhs, const Rhs& rhs)
        : super_t{make_mutable_automaton(make_context_(lhs, rhs)), lhs, rhs}
      {}

      /// The (accessible part of the) composition of \a lhs_ and \a rhs_.
      void compose()
      {
        initialize_compose();

        if (!Lazy)
          while (!aut_->todo_.empty())
            {
              const auto& p = aut_->todo_.front();
              this->complete_(std::get<1>(p));
              aut_->todo_.pop_front();
            }
      }


      void add_transitions(const state_t src,
                           const state_name_t& psrc)
      {
        add_compose_transitions(src, psrc);
      }

      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;
    private:

      template <Automaton A>
      static auto real_aut(const A& aut)
      {
        return aut;
      }

      template <Automaton A>
      static auto real_aut(const insplit_automaton<A>& aut)
      {
        return aut->aut_out();
      }

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <Automaton A>
      using transition_map_t
        = transition_map<A, weightset_t, false, true, true>;

      static labelset_t make_labelset_(const hidden_l_labelset_t& ll,
                                       const hidden_r_labelset_t& rl)
      {
        constexpr auto lsize = hidden_l_labelset_t::size();
        constexpr auto rsize = hidden_r_labelset_t::size();
        return make_labelset_(ll, make_index_sequence<lsize>{},
                              rl, make_index_sequence<rsize>{});
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
        return {make_labelset_(lhs->res_labelset(),
                               real_aut(rhs)->res_labelset()),
                join(*lhs->weightset(), *rhs->weightset())};
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the composition algorithm.
      void initialize_compose()
      {
        aut_->todo_.emplace_back(aut_->pre_(), aut_->pre());
      }

      res_label_t join_label(const hidden_l_label_t& ll,
                             const hidden_r_label_t& rl) const
      {
        return std::tuple_cat(ll, rl);
      }

      template <Automaton Aut>
      std::enable_if_t<labelset_t_of<Aut>::has_one(),
                       res_label_t_of<Aut>>
      get_hidden_one(const Aut& aut) const
      {
        return aut->hidden_one();
      }

      template <Automaton Aut>
      std::enable_if_t<labelset_t_of<Aut>::has_one(),
                        res_label_t_of<Aut>>
      get_hidden_one(const insplit_automaton<Aut>& aut) const
      {
        return real_aut(aut)->hidden_one();
      }

      template <Automaton Aut>
      ATTRIBUTE_NORETURN
      std::enable_if_t<!labelset_t_of<Aut>::has_one(),
                       res_label_t_of<Aut>>
      get_hidden_one(const Aut&) const
      {
        raise("should not get here");
      }

      using super_t::transition_maps_;
      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_compose_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        const auto& lhs = std::get<0>(aut_->auts_);
        const auto& rhs = std::get<1>(aut_->auts_);

        // Outgoing transition cache.
        const auto& ltm = std::get<0>(transition_maps_)[std::get<0>(psrc)];
        const auto& rtm = std::get<1>(transition_maps_)[std::get<1>(psrc)];

        bool has_eps_out = false;
        if (!is_spontaneous_in(rhs, std::get<1>(psrc)))
          has_eps_out = add_one_transitions_<0>(src, psrc);

        // If lhs did not issue spontaneous transitions but has proper
        // transitions, issue follow all the rhs spontaneous
        // transitions.
        const bool lhs_has_proper_trans =
          !ltm.empty()
          && (!lhs->labelset()->is_one(ltm.begin()->first)
              || 2 <= ltm.size());

        if (!has_eps_out || lhs_has_proper_trans)
          add_one_transitions_<1>(src, psrc);

        // In order to avoid having to call add_transition each time, we cache
        // the transitions we add using a polynomial. We conserve a polynomial
        // for each successor of src.
        using polynomialset_t
          = polynomialset<context_t, wet_kind_t::unordered_map>;
        using polynomial_t = typename polynomialset_t::value_t;
        const auto ps = polynomialset_t(aut_->context());
        auto poly_maps = std::map<state_t, polynomial_t>();

        for (auto t: zip_maps(ltm, rtm))
          // The type of the common label is that of the visible tape
          // of either automata.
          if (!lhs->labelset()->is_one(t.first))
            // These may not be new transitions: as the automata are focus,
            // there might be two transitions with the same destination and
            // label, with only the hidden part that changes.
            cross_tuple
              ([&] (const typename transition_map_t<Lhs>::transition& lts,
                    const typename transition_map_t<Rhs>::transition& rts)
               {
                 // Cache the transitions.
                 ps.add_here(poly_maps[this->state(lts.dst, rts.dst)],
                             join_label(lhs->hidden_label_of(lts.transition),
                                        real_aut(rhs)->hidden_label_of(rts.transition)),
                             this->weightset()->mul(lts.weight(), rts.weight()));
               },
               t.second);

        // For each successor, add a transition for each monomial of the
        // corresponding polynomial.
        for (auto elt: poly_maps)
          for (auto m: elt.second)
            this->new_transition(src, elt.first, m.first, m.second);
      }


      template <std::size_t I>
      bool
      add_one_transitions_(const state_t src, const state_name_t& psrc)
      {
        const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];

        // "Loop" only on the spontaneous transitions.  "One" is
        // guaranteed to be first in the transition maps.
        if (tmap.empty()
            || !std::get<I>(aut_->auts_)->labelset()->is_one(tmap.begin()->first))
          return false;
        else
          {
            for (auto t: tmap.begin()->second)
              {
                // Tuple of destination states.
                auto pdst =
                  static_if<I == 0>
                  ([dst=t.dst, &psrc]{
                    return std::make_tuple(dst, std::get<1>(psrc));
                  },
                   [dst=t.dst, &psrc]{
                     return std::make_tuple(std::get<0>(psrc), dst);
                   })();
                // Label.
                auto lbl =
                  static_if<I == 0>
                  ([this, t=t.transition](const auto& lhs, const auto& rhs)
                   {
                     return join_label(lhs->hidden_label_of(t),
                                       get_hidden_one(rhs->aut_out()));
                   },
                   [this, t=t.transition](const auto& lhs, const auto& rhs)
                   {
                     return join_label(get_hidden_one(lhs),
                                       real_aut(rhs)->hidden_label_of(t));
                   })
                  (std::get<0>(aut_->auts_), std::get<1>(aut_->auts_));
                this->new_transition(src, this->state(pdst), lbl, t.weight());
              }
            return true;
          }
      }

      template <Automaton Aut>
      std::enable_if_t<labelset_t_of<Aut>::has_one(), bool>
      is_one(const Aut& aut, transition_t_of<Aut> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      template <Automaton Aut>
      constexpr
      std::enable_if_t<!labelset_t_of<Aut>::has_one(), bool>
      is_one(const Aut&, transition_t_of<Aut>) const
      {
        return false;
      }

      /// Check if the state has only incoming spontaneous
      /// transitions.  As it is in the case of the one-free labelset,
      /// it's always false.
      template <Automaton Aut>
      constexpr
      std::enable_if_t<!labelset_t_of<Aut>::has_one(), bool>
      is_spontaneous_in(const Aut&, state_t_of<Aut>) const
      {
        return false;
      }

      /// Whether the state has only incoming spontaneous transitions.
      /// The automaton has been insplit, so either all incoming transitions
      /// are proper, or all transitions are spontaneous (including the first
      /// one).
      template <Automaton Aut>
      std::enable_if_t<labelset_t_of<Aut>::has_one(), bool>
      is_spontaneous_in(const Aut& rhs, state_t_of<Aut> rst) const
      {
        auto rin = all_in(rhs, rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr);
      }
    };
  }

  /// A compose automaton as a shared pointer.
  template <bool Lazy, Automaton Lhs, Automaton Rhs>
  using compose_automaton
    = std::shared_ptr<detail::compose_automaton_impl<Lazy, Lhs, Rhs>>;

  template <bool Lazy, std::size_t OutTape, std::size_t InTape,
            Automaton Lhs, Automaton Rhs>
  auto
  make_compose_automaton(const Lhs& lhs, const Rhs& rhs)
  {
    auto l = focus<OutTape>(lhs);
    auto r = insplit_lazy(focus<InTape>(rhs));
    using res_t = compose_automaton<Lazy,
                                    focus_automaton<OutTape, Lhs>,
                                    decltype(r)>;
    return make_shared_ptr<res_t>(l, r);
  }

  /*--------------------------------.
  | compose(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) composition.
  template <Automaton Lhs, Automaton Rhs,
            std::size_t OutTape = 1, std::size_t InTape = 0>
  auto
  compose(Lhs& lhs, Rhs& rhs)
  {
    auto res = make_compose_automaton<false, OutTape, InTape>(lhs, rhs);
    res->compose();
    return res->strip();
  }

  /// Build the (accessible part of the) laze composition.
  template <typename Lhs, typename Rhs,
            std::size_t OutTape = 1, std::size_t InTape = 0>
  auto
  compose_lazy(Lhs& lhs, Rhs& rhs)
  {
    auto res = make_compose_automaton<true, OutTape, InTape>(lhs, rhs);
    res->compose();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Lhs, Automaton Rhs, typename Bool>
      automaton
      compose(automaton& lhs, automaton& rhs, bool lazy)
      {
        auto& l = lhs->as<Lhs>();
        auto& r = rhs->as<Rhs>();
        if (lazy)
          return ::vcsn::compose_lazy(l, r);
        else
          return ::vcsn::compose(l, r);
      }
    }
  }
}
