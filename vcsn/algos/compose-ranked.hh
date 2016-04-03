#pragma once

#include <vcsn/algos/focus.hh>
#include <vcsn/core/lazy-tuple-automaton.hh>
#include <vcsn/core/mutable-automaton.hh>
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
    using Type ## _of                                                     \
      = typename Type ## _of_impl<Aut>::type

    DEFINE(res_labelset_t);
    DEFINE(res_label_t);
    DEFINE(full_context_t);

#undef DEFINE

    /// Build the (accessible part of the) composition.
    template <Automaton Lhs, Automaton Rhs>
    struct composed2_type
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
    class compose2_automaton_impl
      : public lazy_tuple_automaton<compose2_automaton_impl<Lazy, Lhs, Rhs>,
                                    any_<labelset_t_of<Lhs>::has_one(),
                                         labelset_t_of<Rhs>::has_one()>(),
                                    true, // KeepTransitions.
                                    Lazy,
                                    typename composed2_type<Lhs, Rhs>::out_t,
                                    Lhs, Rhs>
    {
      static_assert(full_context_t_of<Lhs>::is_lat,
                    "compose2: lhs labelset must be a tupleset");
      static_assert(full_context_t_of<Rhs>::is_lat,
                    "compose2: rhs labelset must be a tupleset");

      static_assert(std::is_same<labelset_t_of<Lhs>,
                                 labelset_t_of<Rhs>>::value,
                    "compose2: common tape must be of same type");

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

    public:

      using self_t = compose2_automaton_impl;

      using type_helper_t = composed2_type<Lhs, Rhs>;
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
      /// Whether states have a rank slot.
      constexpr static bool ranked = any_<labelset_t_of<Lhs>::has_one(),
                                          labelset_t_of<Rhs>::has_one()>();
      using super_t = lazy_tuple_automaton<self_t, ranked, true, Lazy, out_t,
                                           Lhs, Rhs>;
      /// Result state type.
      using state_t = typename super_t::state_t;
      /// Tuple of states of input automata.
      using state_name_t = typename super_t::state_name_t;

      using super_t::aut_;

      static symbol sname()
      {
        static symbol res(std::string{"compose2_automaton<"}
                          + (Lazy ? "true" : "false") + ", "
                          + Lhs::element_type::sname() + ", "
                          + Rhs::element_type::sname() + ">");
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "compose2_automaton<";
        std::get<0>(aut_->auts_)->print_set(o, fmt) << ", ";
        return std::get<1>(aut_->auts_)->print_set(o, fmt) << ">";
      }

      compose2_automaton_impl(const Lhs& lhs, const Rhs& rhs)
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
              add_compose_transitions(std::get<1>(p), std::get<0>(p));
              aut_->todo_.pop();
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

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <Automaton A>
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
        return {make_labelset_(lhs->res_labelset(),
                               rhs->res_labelset()),
                join(*lhs->weightset(), *rhs->weightset())};
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the composition algorithm.
      void initialize_compose()
      {
        aut_->todo_.push({aut_->pre_(), aut_->pre()});
        aut_->emplace(aut_->pre_(), aut_->pre());
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

        if (static_if<ranked>([](const auto& psrc){ return std::get<2>(psrc) <= 0; },
                              [](const auto& psrc){ return true; })(psrc)
            // "Loop" only on the spontaneous transitions.  "One" is
            // guaranteed to be first in the transition maps.
            && !ltm.empty()
            && lhs->labelset()->is_one(ltm.begin()->first))
          // for each spontaneous transitions leaving the state
          for (auto t: ltm.begin()->second)
            {
              state_t dst = static_if<ranked>
                ([this](auto... s) { return this->state(s..., 0); },
                 [this](auto... s) { return this->state(s...); })
                (t.dst, std::get<1>(psrc));
              this->new_transition(src, dst,
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

        if (lhs_has_proper_trans
            && !rtm.empty()
            && rhs->labelset()->is_one(rtm.begin()->first))
          for (auto t: rtm.begin()->second)
            {
              const state_t dst = static_if<ranked>
                ([this](auto... s) { return this->state(s..., 1); },
                 [this](auto... s) { return this->state(s...); })
                (std::get<0>(psrc), t.dst);
              this->new_transition(src, dst,
                                   join_label(get_hidden_one(lhs),
                                              rhs->hidden_label_of(t.transition)),
                                   t.weight());
            }


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
                 const state_t dst = static_if<ranked>
                   ([this](auto... s) { return this->state(s..., 0); },
                    [this](auto... s) { return this->state(s...); })
                   (lts.dst, rts.dst);
                 // Cache the transitions.
                 ps.add_here(poly_maps[dst],
                             join_label(lhs->hidden_label_of(lts.transition),
                                        rhs->hidden_label_of(rts.transition)),
                             this->weightset()->mul(lts.weight(), rts.weight()));
               },
               t.second);

        // For each successor, add a transition for each monomial of the
        // corresponding polynomial.
        for (const auto& elt: poly_maps)
          for (const auto& m: elt.second)
            this->new_transition(src, elt.first, m.first, m.second);
      }

      /// If the labelset has one, add the relevant spontaneous
      /// transitions leaving the state.
      template <std::size_t I, typename L>
      std::enable_if_t<L::has_one(), void>
      maybe_add_one_transitions_(const L& ls, const state_t src,
                                 const state_name_t& psrc)
      {
        if (!has_one_in(psrc, I + 1, aut_->indices)
            && !has_only_one_out(psrc, I, aut_->indices))
          {
            // one is guaranteed to be first.
            const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
            if (!tmap.empty() && ls.is_one(tmap.begin()->first))
              for (auto t : tmap.begin()->second)
                {
                  auto pdst = psrc;
                  std::get<I>(pdst) = t.dst;
                  this->new_transition(src, state(pdst), ls.one(), t.weight());
                }
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
    };
  }

  /// A compose automaton as a shared pointer.
  template <bool Lazy, Automaton Lhs, Automaton Rhs>
  using compose2_automaton
    = std::shared_ptr<detail::compose2_automaton_impl<Lazy, Lhs, Rhs>>;

  template <bool Lazy, std::size_t OutTape, std::size_t InTape,
            Automaton Lhs, Automaton Rhs>
  auto
  make_compose2_automaton(const Lhs& lhs, const Rhs& rhs)
  {
    auto l = focus<OutTape>(lhs);
    auto r = focus<InTape>(rhs);
    using res_t = compose2_automaton<Lazy,
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
  compose2(Lhs& lhs, Rhs& rhs)
  {
    auto res = make_compose2_automaton<false, OutTape, InTape>(lhs, rhs);
    res->compose();
    return res->strip();
  }

  /// Build the (accessible part of the) laze composition.
  template <typename Lhs, typename Rhs,
            std::size_t OutTape = 1, std::size_t InTape = 0>
  auto
  compose2_lazy(Lhs& lhs, Rhs& rhs)
  {
    auto res = make_compose2_automaton<true, OutTape, InTape>(lhs, rhs);
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
      compose2(automaton& lhs, automaton& rhs, bool lazy)
      {
        auto& l = lhs->as<Lhs>();
        auto& r = rhs->as<Rhs>();
        if (lazy)
          return ::vcsn::compose2_lazy(l, r);
        else
          return ::vcsn::compose2(l, r);
      }
    }
  }
}
