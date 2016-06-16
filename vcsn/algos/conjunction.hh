#pragma once

#include <iostream>
#include <map>
#include <utility>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/insplit.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/tags.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/core/lazy-tuple-automaton.hh>
#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/set.hh> // has
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/to.hh>
#include <vcsn/misc/tuple.hh> // tuple_element_t, cross_tuple
#include <vcsn/misc/zip-maps.hh>

namespace vcsn
{
  namespace detail
  {
    /*---------------------------------.
    | product_automaton_impl<Aut...>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) product.
    template <bool Lazy, Automaton Aut, Automaton... Auts>
    class product_automaton_impl
      : public lazy_tuple_automaton<product_automaton_impl<Lazy, Aut, Auts...>,
                                    false, Lazy, Aut, Auts...>
    {
      static_assert(all_<labelset_t_of<Auts>::is_letterized()...>(),
                    "product: requires letterized labels");

      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using self_t = product_automaton_impl;
      using super_t = lazy_tuple_automaton<self_t, false, Lazy, Aut, Auts...>;

    public:
      using state_name_t = typename super_t::state_name_t;
      using state_t = typename super_t::state_t;

      template <Automaton A>
      using transition_map_t = typename super_t::template transition_map_t<A>;

      template <size_t... I>
      using seq = typename super_t::template seq<I...>;

      using super_t::ws_;
      using super_t::transition_maps_;

      static symbol sname()
      {
        static symbol res("product_automaton"
                          + super_t::sname_(std::string{Lazy ? "true" : "false"}));
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "product_automaton";
        return aut_->print_set_(o, fmt);
      }

      /// The context of the result.
      using context_t = context_t_of<Aut>;
      using labelset_t = labelset_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;

      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// The type of input automata.
      using automata_t = std::tuple<Auts...>;

      /// The type of the Ith input automaton, unqualified.
      template <size_t I>
      using input_automaton_t = base_t<tuple_element_t<I, automata_t>>;

      using super_t::aut_;

      /// Build a product automaton.
      /// \param aut   the automaton to build.
      /// \param auts  the input automata.
      product_automaton_impl(Aut aut, const Auts&... auts)
        : super_t{aut, auts...}
      {}

      /// Compute the (accessible part of the) conjunction.
      void conjunction()
      {
        initialize_conjunction();

        if (!Lazy)
          while (!aut_->todo_.empty())
            {
              const auto& p = aut_->todo_.front();
              add_conjunction_transitions(std::get<1>(p), std::get<0>(p));
              aut_->todo_.pop_front();
            }
      }

      /// Compute the left quotient
      template <bool L = Lazy>
      std::enable_if_t<sizeof...(Auts) == 2 && !L> ldivide()
      {
        static_assert(labelset_t::has_one(),
                      "ldivide: labelset must have a neutral");
        initialize_conjunction();

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            add_ldiv_transitions(std::get<1>(p), std::get<0>(p));
            aut_->todo_.pop_front();
          }
      }

      /// Compute the deterministic sum of two deterministic automata.
      template <bool L = Lazy>
      std::enable_if_t<sizeof...(Auts) == 2 && !L> add()
      {
        using lhs_t = input_automaton_t<0>;
        using rhs_t = input_automaton_t<1>;
        constexpr bool bb = (std::is_same<weightset_t_of<lhs_t>, b>::value
                             && std::is_same<weightset_t_of<rhs_t>, b>::value);
        static_assert(bb, "add: requires Boolean weightset");
        initialize_conjunction();

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            add_add_transitions(std::get<1>(p), std::get<0>(p));
            aut_->todo_.pop_front();
          }
      }

      /// Compute the left quotient in-place.
      template <bool L = Lazy>
      std::enable_if_t<sizeof...(Auts) == 2 && !L> ldiv_here()
      {
        initialize_conjunction();

        using rhs_t = input_automaton_t<1>;
        auto new_initials = std::vector<state_t_of<rhs_t>>();

        const auto& lhs = std::get<0>(aut_->auts_);
        auto& rhs = std::get<1>(aut_->auts_);

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            const auto& state_name = std::get<0>(p);
            add_conjunction_transitions(std::get<1>(p), state_name);

            // If lhs's state is final, rhs's corresponding state is initial.
            if (lhs->is_final(std::get<0>(state_name)))
              new_initials.push_back(std::get<1>(state_name));
            aut_->todo_.pop_front();
          }

        for (auto t: initial_transitions(rhs))
          rhs->unset_initial(rhs->dst_of(t));
        for (auto s: new_initials)
          rhs->set_initial(s);
      }

      /// Compute the (accessible part of the) shuffle product.
      void shuffle()
      {
        initialize_shuffle();

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            add_shuffle_transitions<false>(std::get<1>(p), std::get<0>(p));
            aut_->todo_.pop_front();
          }
      }

      /// Compute the (accessible part of the) infiltration product.
      void infiltrate()
      {
        // Variadic infiltrate is not trivial to implement, it's not
        // just conjunction and shuffle in series.  For instance, consider
        // three automata:
        //
        //           <x>a
        // x = -> 0 ------> 1 ->
        //
        // and likewise for y and z.  Let's use `&:` to denote
        // infiltrate.  In (x &: y) there is a transition ((0,0),
        // <xy>a, (1,1)) coming from the conjunction-like transitions.
        //
        // Therefore in (x &: y) &: z there is a transition ((0,0),0),
        // <xy>a, (1,1), 0) by virtue of the shuffle-like transitions.
        //
        // This kind of transition that mixes conjunction and shuffle
        // would never appear in a naive implementation with only
        // conjunction and shuffle transitions, but no combinations.
        require(sizeof...(Auts) == 2,
                "infiltrate: variadic product does not work");

        // Infiltrate is a mix of conjunction and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for conjunction:
        initialize_shuffle();

        while (!aut_->todo_.empty())
          {
            const auto& p = aut_->todo_.front();
            // Infiltrate is a mix of conjunction and shuffle operations.
            //
            // Conjunction transitions must be added before shuffle ones:
            // this way "conjunction" can use "new_transition" only, which
            // is faster than "add_transition".
            add_conjunction_transitions(std::get<1>(p), std::get<0>(p));
            add_shuffle_transitions<true>(std::get<1>(p), std::get<0>(p));

            aut_->todo_.pop_front();
          }
      }

      /// Tell lazy_tuple_automaton how to add the transitions to a state
      void add_transitions(const state_t src,
                           const state_name_t& psrc)
      {
        add_conjunction_transitions(src, psrc);
      }

    private:
      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the conjunction algorithm.
      void initialize_conjunction()
      {
        aut_->todo_.emplace_back(aut_->pre_(), aut_->pre());
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle()
      {
        // Make the result automaton initial states: same as the
        // conjunction of pre: synchronized transitions on $.
        add_conjunction_transitions(aut_->pre(), aut_->pre_());
      }

      using super_t::out_;
      using super_t::state;
      /// Add transitions to the result automaton, starting from the
      /// given result input state, which must correspond to the given
      /// pair of input state automata.  Update the worklist with the
      /// needed source-state pairs.
      void add_conjunction_transitions(const state_t src,
                                       const state_name_t& psrc)
      {
        for (const auto& t: zip_map_tuple(out_(psrc)))
          // These are always new transitions: first because the
          // source state is visited for the first time, and second
          // because the couple (left destination, label) is unique,
          // and so is (right destination, label).
          if (!aut_->labelset()->is_one(t.first))
            detail::cross_tuple
              ([this,src,&t]
               (const typename transition_map_t<Auts>::transition&... ts)
               {
                 this->new_transition(src, state(ts.dst...),
                                      t.first,
                                      ws_.mul(ts.weight()...));
               },
               t.second);
        add_one_transitions_(src, psrc, aut_->indices);
      }

      /// Behave similarly to add_conjunction_transitions, with three main
      /// differences: the algorithm continues matching the right hand side
      /// even when the left hand side has reached post, the labels are set to
      /// one when the right hand side and left hand side match (that is,
      /// before the left hand side reaches post), and the weights are divided
      /// rather than multiplied.
      template <bool L = Lazy>
      std::enable_if_t<sizeof...(Auts) == 2 && !L>
      add_ldiv_transitions(const state_t src, const state_name_t& psrc)
      {
        const auto& ls = *aut_->labelset();
        const auto& lhs = std::get<0>(aut_->auts_);
        const auto& rhs = std::get<1>(aut_->auts_);
        const auto& lstate = std::get<0>(psrc);
        const auto& rstate = std::get<1>(psrc);

        if (lhs->is_final(lstate) || lstate == lhs->post())
        {
          for (auto ts: all_out(rhs, rstate))
          {
            const auto& lweight = lhs->is_final(lstate)
              ? lhs->get_final_weight(lstate) : ws_.one();
            this->new_transition(src,
                                 state(lhs->post(), rhs->dst_of(ts)),
                                 rhs->label_of(ts),
                                 ws_.ldivide(lweight, rhs->weight_of(ts)));
          }
        }
        for (const auto& t: zip_map_tuple(out_(psrc)))
          if (!ls.is_one(t.first)
              && (!ls.is_special(t.first) || src == aut_->pre()))
            detail::cross_tuple
              ([this,ls,src,t]
               (const typename transition_map_t<Auts>::transition&... ts)
               {
                 this->add_transition(src, state(ts.dst...),
                                      ls.is_special(t.first)
                                      ? t.first : ls.one(),
                                      ws_.ldivide(ts.weight()...));
               },
               t.second);
      }

      /// Behaves similarly to add_conjunction_transitions on a Boolean
      /// weightset, but use post() as a special state that matches everything
      /// when one of the two automata does not match on a label.
      template <bool L = Lazy>
      std::enable_if_t<sizeof...(Auts) == 2 && !L>
      add_add_transitions(const state_t src, const state_name_t& psrc)
      {
        const auto& lhs = std::get<0>(aut_->auts_);
        const auto& rhs = std::get<1>(aut_->auts_);
        const auto& lstate = std::get<0>(psrc);
        const auto& rstate = std::get<1>(psrc);

        auto common_labels = std::set<label_t_of<Aut>>{};
        for (auto t: zip_map_tuple(out_(psrc)))
        {
          common_labels.insert(t.first);
          detail::cross_tuple
            ([this,src,t]
             (const typename transition_map_t<Auts>::transition&... ts)
             {
             this->add_transition(src, state(ts.dst...), t.first);
             },
             t.second);
        }
        for (auto t: all_out(lhs, lstate))
          if (!has(common_labels, lhs->label_of(t)))
            this->new_transition(src, state(lhs->dst_of(t), rhs->post()),
                  lhs->label_of(t));
        for (auto t: all_out(rhs, rstate))
          if (!has(common_labels, rhs->label_of(t)))
            this->new_transition(src, state(lhs->post(), rhs->dst_of(t)),
                  rhs->label_of(t));
      }

      /// Add the spontaneous transitions leaving state \a src, if it
      /// is relevant (i.e. only for the labelsets that have one).
      template <std::size_t... I>
      void
      add_one_transitions_(const state_t src, const state_name_t& psrc,
                           seq<I...>)
      {
        using swallow = int[];
        (void) swallow
        {
          (add_one_transitions_<I>(*(std::get<I>(aut_->auts_)->labelset()),
                                   src, psrc), 0)...
        };
      }

      /// In the case where the labelset doesn't have one, do nothing.
      template <std::size_t I, typename LS>
      std::enable_if_t<!LS::has_one(), void>
      add_one_transitions_(const LS&, const state_t, const state_name_t&)
      {}

      /// If the I-th labelset has one, add the relevant spontaneous
      /// transitions leaving the state.
      template <std::size_t I, typename LS>
      std::enable_if_t<LS::has_one(), void>
      add_one_transitions_(const LS& ls, const state_t src,
                           const state_name_t& psrc)
      {
        // The first condition prevents the creation of redundant
        // paths that would lead to incorrect valuations (in the
        // weighted case), while the second is purely an optimization,
        // avoiding the creation of non-coaccessible states.
        if (are_proper_in(psrc, make_index_range<I + 1, sizeof...(Auts)>{})
            && have_proper_out(psrc, make_index_range<0, I>{}))
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

      /// Whether no tapes in the sequence have spontaneous incoming
      /// transitions.
      template <std::size_t... I>
      bool are_proper_in(const state_name_t& psrc, seq<I...>) const
      {
        return all(is_proper_in<I>(psrc)...);
      }

      /// Whether all the tapes in the sequence have proper outgoing
      /// transitions (but possibly spontaneous too).
      template <std::size_t... I>
      bool have_proper_out(const state_name_t& psrc, seq<I...>)
      {
        return all(has_proper_out<I>(psrc)...);
      }

      /// Check if the transition is spontaneous (in the case of a
      /// labelset with one).
      template <Automaton Aut_>
      std::enable_if_t<labelset_t_of<Aut_>::has_one(), bool>
      is_one(const Aut_& aut, transition_t_of<Aut_> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      /// Same as above, but for labelsets without one, so it's always
      /// false.
      template <Automaton Aut_>
      constexpr std::enable_if_t<!labelset_t_of<Aut_>::has_one(), bool>
      is_one(const Aut_&, transition_t_of<Aut_>) const
      {
        return false;
      }

      /// Whether the state has only proper incoming transitions.
      template <size_t I>
      constexpr auto
      is_proper_in(const state_name_t&) const
        -> std::enable_if_t<!labelset_t_of<input_automaton_t<I>>::has_one(),
                            bool>
      {
        return true;
      }

      /// Whether the state has only proper incoming transitions.  The
      /// automaton has been insplit, so either all incoming
      /// transitions are proper, or all transitions are spontaneous
      /// (including the first one).
      template <size_t I>
      auto
      is_proper_in(const state_name_t& sn) const
        -> std::enable_if_t<labelset_t_of<input_automaton_t<I>>::has_one(),
                            bool>
      {
        // Amusingly enough, it is faster to check the incoming
        // transitions rather than recovering the decoration of the
        // insplit state, which tells whether the state is proper-in.
        const auto& aut = std::get<I>(aut_->auts_);
        auto s = std::get<I>(sn);
        auto rin = all_in(aut, s);
        auto rtr = rin.begin();
        // Insplit state, so checking the first transition suffices.
        // There can be no incoming transitions in the case of pre.
        return rtr == rin.end() || !is_one(aut, *rtr);
      }

      /// Whether the Ith state of \a psrc in the Ith input automaton
      /// has proper outgoing transitions (but possibly spontaneous
      /// transitions too).
      ///
      /// Not const, because we (might) update the transition maps.
      template <size_t I>
      bool
      has_proper_out(const state_name_t& psrc)
      {
        const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
        auto s = tmap.size();
        if (s == 0)
          return false;
        else if (2 <= s)
          return true;
        else
          return !std::get<I>(aut_->auts_)->labelset()->is_one(tmap.begin()->first);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given tuple of input state automata.  Update the worklist
      /// with the needed source-state pairs.
      template <bool Infiltrate = false>
      void add_shuffle_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        weight_t final
          = add_shuffle_transitions_<Infiltrate>(src, psrc, aut_->indices);
        aut_->set_final(src, final);
      }

      /// Let all automata advance one after the other, and add the
      /// corresponding transitions in the output.
      ///
      /// Return the product of the final states.
      template <bool Infiltrate, size_t... I>
      weight_t add_shuffle_transitions_(const state_t src,
                                        const state_name_t& psrc,
                                        seq<I...>)
      {
        weight_t res = ws_.one();
        using swallow = int[];
        (void) swallow
        {
          (res = ws_.mul(res,
                         add_shuffle_transitions_<Infiltrate, I>(src, psrc)),
           0)...
        };
        return res;
      }

      /// Let Ith automaton advance, and add the corresponding
      /// transitions in the output.
      ///
      /// If we reach a final state, return the corresponding final
      /// weight (zero otherwise).
      ///
      /// \tparam Infiltrate
      ///    whether we are called after add_conjunction_transitions.
      /// \tparam I
      ///    the tape on which to perform a transition.
      template <bool Infiltrate, size_t I>
      weight_t
      add_shuffle_transitions_(const state_t src,
                               const state_name_t& psrc)
      {
        // Whether is a final state.
        weight_t res = ws_.zero();

        auto& ts = std::get<I>(transition_maps_)[std::get<I>(psrc)];
        for (auto t: ts)
          if (std::get<I>(aut_->auts_)->labelset()->is_special(t.first))
            res = t.second.front().weight();
          else
            // The src state is visited for the first time, so all
            // these transitions are new.  *Except* in the case where
            // we have a loop on some tapes.
            //
            // If add_conjunction_transitions was called before (in the
            // case of infiltrate), there may even exist such a
            // transition in the first loop.
            //
            // To trigger the later case, try the self-infiltrate on
            // derived_term('a*a').
            for (auto d: t.second)
              {
                auto pdst = psrc;
                std::get<I>(pdst) = d.dst;
                if (Infiltrate
                    || std::get<I>(psrc) == d.dst)
                  this->add_transition(src, state(pdst), t.first, d.weight());
                else
                  this->new_transition(src, state(pdst), t.first, d.weight());
              }
        return res;
      }
    };

    /// A product automaton as a shared pointer.
    template <bool Lazy, Automaton Aut, Automaton... Auts>
    using product_automaton
      = std::shared_ptr<detail::product_automaton_impl<Lazy, Aut, Auts...>>;

    template <bool Lazy, Automaton Aut, Automaton... Auts>
    auto
    make_product_automaton(Aut aut, const Auts&... auts)
      -> product_automaton<Lazy, Aut, Auts...>
    {
      using res_t = product_automaton<Lazy, Aut, Auts...>;
      return make_shared_ptr<res_t>(aut, auts...);
    }


    /*-----------------------------.
    | conjunction(automaton...).   |
    `-----------------------------*/

    /// Build the (accessible part of the) conjunction.
    template <Automaton Aut, Automaton... Auts>
    auto
    conjunction(const Aut& a, const Auts&... as)
    {
      auto res = make_product_automaton<false>(meet_automata(a, as...),
                                               a, insplit(as)...);
      res->conjunction();
      return res->strip();
    }

    /// Build the (accessible part of the) conjunction, on-the-fly.
    template <Automaton Aut, Automaton... Auts>
    auto
    conjunction_lazy(const Aut& a, const Auts&... as)
    {
      auto res = make_product_automaton<true>(meet_automata(a, as...),
                                              a, insplit(as)...);
      res->conjunction();
      return res;
    }
  }

  using detail::conjunction;
  using detail::conjunction_lazy;

  namespace dyn
  {
    namespace detail
    {
      /// Bridge helper.
      template <typename Auts, size_t... I>
      automaton
      conjunction_(const std::vector<automaton>& as, bool lazy,
                   vcsn::detail::index_sequence<I...>)
      {
        if (lazy)
          return conjunction_lazy(as[I]->as<tuple_element_t<I, Auts>>()...);
        else
          return conjunction(as[I]->as<tuple_element_t<I, Auts>>()...);
      }

      /// Bridge (conjunction).
      template <typename Auts, typename Bool>
      automaton
      conjunction(const std::vector<automaton>& as, bool lazy)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return conjunction_<Auts>(as, lazy, indices);
      }
    }
  }


  /*-----------------------------.
  | ldivide(automaton, automaton).  |
  `-----------------------------*/

  /// Compute the left quotient.
  ///
  /// \param lhs  left hand side
  /// \param rhs  right hand side
  template <Automaton Aut1, Automaton Aut2>
  auto
  ldivide(const Aut1& lhs, const Aut2& rhs, auto_tag = {})
  {
    return detail::static_if<std::is_same<weightset_t_of<Aut1>, b>::value
                             && std::is_same<weightset_t_of<Aut2>, b>::value>
             ([] (const auto& lhs, const auto& rhs)
              {
                return ldivide(lhs, rhs, boolean_tag{});
              },
              [] (const auto& lhs, const auto& rhs)
              {
                return ldivide(lhs, rhs, weighted_tag{});
              }
             )(lhs, rhs);
  }

  template <Automaton Aut1, Automaton Aut2>
  auto
  ldivide(const Aut1& lhs, const Aut2& rhs, boolean_tag)
  {
    auto res = insplit(rhs);
    auto prod =
      detail::make_product_automaton<false>(join_automata(lhs, res), lhs, res);
    prod->ldiv_here();
    return res;
  }

  template <Automaton Aut1, Automaton Aut2>
  auto
  ldivide(const Aut1& lhs, const Aut2& rhs, weighted_tag)
  {
    auto prod =
      detail::make_product_automaton<false>(join_automata(lhs, rhs), lhs, rhs);
    prod->ldivide();
    return prod->strip();
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (ldivide).
      template <Automaton Aut1, Automaton Aut2>
      automaton
      ldivide(const automaton& aut1, const automaton& aut2)
      {
        const auto& a1 = aut1->as<Aut1>();
        const auto& a2 = aut2->as<Aut2>();
        return vcsn::ldivide<Aut1, Aut2>(a1, a2);
      }
    }
  }



  /*-----------------------------.
  | rdivide(automaton, automaton).  |
  `-----------------------------*/

  /// Compute the right quotient.
  ///
  /// \param a1  left hand side
  /// \param a2  right hand side
  template <Automaton Aut1, Automaton Aut2>
  auto
  rdivide(const Aut1& a1, const Aut2& a2)
  {
    auto a1t = transpose(a1);
    auto a2t = transpose(a2);
    return transpose(ldivide(a2t, a1t));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut1, Automaton Aut2>
      automaton
      rdivide(const automaton& aut1, const automaton& aut2)
      {
        const auto& a1 = aut1->as<Aut1>();
        const auto& a2 = aut2->as<Aut2>();
        return vcsn::rdivide(a1, a2);
      }
    }
  }


  /*------------------------.
  | shuffle(automaton...).  |
  `------------------------*/

  /// The (accessible part of the) shuffle product.
  template <Automaton... Auts>
  auto
  shuffle(const Auts&... as)
    // SFINAE
    -> tuple_automaton<decltype(join_automata(as...)),
                       Auts...>
  {
    auto res =
      detail::make_product_automaton<false>(join_automata(as...), as...);
    res->shuffle();
    return res->strip();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Variadic bridge helper.
      template <typename Auts, size_t... I>
      automaton
      shuffle_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        return vcsn::shuffle(as[I]->as<tuple_element_t<I, Auts>>()...);
      }

      /// Bridge (shuffle).
      template <typename Auts>
      automaton
      shuffle(const std::vector<automaton>& as)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return shuffle_<Auts>(as, indices);
      }
    }
  }


  /*-----------------------------------.
  | shuffle(expression, expression).   |
  `-----------------------------------*/

  /// Shuffle product of expressions.
  template <typename ValueSet>
  typename ValueSet::value_t
  shuffle(const ValueSet& vs,
          const typename ValueSet::value_t& lhs,
          const typename ValueSet::value_t& rhs)
  {
    return vs.shuffle(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (shuffle).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      shuffle_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::shuffle(std::get<0>(join_elts),
                                std::get<1>(join_elts),
                                std::get<2>(join_elts))};
      }
    }
  }


  /*----------------------------.
  | infiltrate(automaton...).   |
  `----------------------------*/

  /// The (accessible part of the) infiltration product.
  template <Automaton A1, Automaton A2>
  auto
  infiltrate(const A1& a1, const A2& a2)
    -> tuple_automaton<decltype(join_automata(a1, a2)),
                       A1, A2>
  {
    auto res =
      detail::make_product_automaton<false>(join_automata(a1, a2), a1, a2);
    res->infiltrate();
    return res->strip();
  }

  /// The (accessible part of the) infiltration product.
  template <Automaton A1, Automaton A2, Automaton A3, Automaton... Auts>
  auto
  infiltrate(const A1& a1, const A2& a2, const A3& a3, const Auts&... as)
    // SFINAE
    -> decltype(infiltrate(infiltrate(a1, a2), a3, as...))
  {
    return infiltrate(infiltrate(a1, a2), a3, as...);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Variadic bridge helper.
      template <typename Auts, size_t... I>
      automaton
      infiltrate_(const std::vector<automaton>& as,
                    vcsn::detail::index_sequence<I...>)
      {
        return vcsn::infiltrate(as[I]->as<tuple_element_t<I, Auts>>()...);
      }

      /// Bridge (infiltrate).
      template <typename Auts>
      automaton
      infiltrate(const std::vector<automaton>& as)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return infiltrate_<Auts>(as, indices);
      }
    }
  }

  /*--------------------------------------.
  | infiltrate(expression, expression).   |
  `--------------------------------------*/

  /// Infiltration product of expressions.
  template <typename ValueSet>
  typename ValueSet::value_t
  infiltrate(const ValueSet& vs,
               const typename ValueSet::value_t& lhs,
               const typename ValueSet::value_t& rhs)
  {
    return vs.infiltrate(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (infiltrate).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      infiltrate_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::infiltrate(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
      }
    }
  }


  /*-----------------------------.
  | conjunction(automaton, n).   |
  `-----------------------------*/

  /// Repeated conjunction of a automaton.
  ///
  /// \param aut  the automaton to conjoin with itself.
  /// \param n    the "power": number of conjunction to apply.
  ///
  /// \returns A non-decorated automaton.
  template <Automaton Aut>
  auto
  conjunction(const Aut& aut, to exp)
    -> fresh_automaton_t_of<Aut>
  {
    // We used to compute `a & n` as `([^]* & a) & a)...`.  The code
    // was simpler, but the additional conjunction (with `[^]*`) made
    // it noticeably slower.  Concrete `a & 2` was twice slower than
    // `a & a`.
    auto res = make_fresh_automaton(aut);
    require(exp.single(),
            "conjunction: exponents must be single, ", exp);
    require(exp.finite(),
            "conjunction: exponents must be finite, ", exp);
    unsigned n = exp.min;
    if (n < 2)
      {
        // automatonset::universal().
        auto s = res->new_state();
        res->set_initial(s);
        res->set_final(s);
        for (auto l: res->context().labelset()->generators())
          res->new_transition(s, s, l);
        if (n == 1)
          // Don't return aut: we need the accessible part.  However,
          // `copy_into(accessible(aut), res)` seems more costly than
          // a plain conjunction!
          res = strip(conjunction(res, aut));
      }
    else
      {
        res = strip(conjunction(aut, aut));
        n -= 2;
        static bool iterative = getenv("VCSN_ITERATIVE");
        if (iterative)
          for (size_t i = 0; i < n; ++i)
            res = strip(conjunction(res, aut));
        else
          {
            auto power = strip(aut);
            while (true)
              {
                if (n % 2)
                  res = strip(conjunction(res, power));
                n /= 2;
                if (!n)
                  break;
                power = strip(conjunction(power, power));
              }
          }
      }

    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <Automaton Aut, typename Unsigned>
      automaton
      conjunction_repeated(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::conjunction(a, to{n});
      }
    }
  }

  /*-----------------------------.
  | conjunction(value, value).   |
  `-----------------------------*/

  /// Intersection/Hadamard product of expressions/polynomials.
  template <typename ValueSet>
  typename ValueSet::value_t
  conjunction(const ValueSet& rs,
              const typename ValueSet::value_t& lhs,
              const typename ValueSet::value_t& rhs)
  {
    return rs.conjunction(lhs, rhs);
  }

  /*---------------------------------------.
  | conjunction(expression, expression).   |
  `---------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      conjunction_expression(const expression& lhs, const expression& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::conjunction(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
      }
    }
  }

  /*-------------------------------------.
  | conjunction(expansion, expansion).   |
  `-------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expansion
      conjunction_expansion(const expansion& lhs, const expansion& rhs)
      {
        auto join_elts = join<ExpSetLhs, ExpSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::conjunction(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
      }
    }
  }

  /*---------------------------------------.
  | conjunction(polynomial, polynomial).   |
  `---------------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      conjunction_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        auto join_elts = join<PolynomialSetLhs, PolynomialSetRhs>(lhs, rhs);
        return {std::get<0>(join_elts),
                ::vcsn::conjunction(std::get<0>(join_elts),
                                    std::get<1>(join_elts),
                                    std::get<2>(join_elts))};
      }
    }
  }
}
