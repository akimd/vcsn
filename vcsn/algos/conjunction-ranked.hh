#pragma once

#include <iostream>
#include <map>
#include <utility>

#include <vcsn/algos/strip.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/core/lazy-tuple-automaton.hh>
#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/value.hh> // dyn::make_expression
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
    class product2_automaton_impl
      : public lazy_tuple_automaton<product2_automaton_impl<Lazy, Aut, Auts...>,
                                    any_<labelset_t_of<Auts>::has_one()...>(),
                                    false, // KeepTransitions.
                                    Lazy, Aut, Auts...>
    {
      static_assert(all_<labelset_t_of<Auts>::is_letterized()...>(),
                    "product: requires letterized labels");

      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using self_t = product2_automaton_impl;
      /// Whether states have a rank slot.
      constexpr static bool ranked = any_<labelset_t_of<Auts>::has_one()...>();
      using super_t
        = lazy_tuple_automaton<self_t, ranked, false, Lazy, Aut, Auts...>;

    public:
      using state_name_t = typename super_t::state_name_t;
      using state_t = typename super_t::state_t;

      template <Automaton A>
      using transition_map_t = typename super_t::template transition_map_t<A>;

      template <size_t... I>
      using seq = typename super_t::template seq<I...>;

      using super_t::ws_;
      using super_t::transition_maps_;

      /// Number of conjoined automata.
      constexpr static size_t rank = sizeof...(Auts);

      static symbol sname()
      {
        static symbol res("product2_automaton"
                          + super_t::sname_(std::string{Lazy ? "true" : "false"}));
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "product2_automaton";
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
      product2_automaton_impl(Aut aut, const Auts&... auts)
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

      /// Compute the left quotient in-place.
      // Division only makes sense for two automata.
      // Lazy version is not implemented yet.
      template <bool L = Lazy>
      std::enable_if_t<rank == 2 && !L> ldiv_here()
      {
        initialize_conjunction();

        using rhs_t = tuple_element_t<1, std::tuple<Auts...>>;
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
      void infiltration()
      {
        // Variadic infiltration is not trivial to implement, it's not
        // just conjunction and shuffle in series.  For instance, consider
        // three automata:
        //
        //           <x>a
        // x = -> 0 ------> 1 ->
        //
        // and likewise for y and z.  Let's use `&:` to denote
        // infiltration.  In (x &: y) there is a transition ((0,0),
        // <xy>a, (1,1)) coming from the conjunction-like transitions.
        //
        // Therefore in (x &: y) &: z there is a transition ((0,0),0),
        // <xy>a, (1,1), 0) by virtue of the shuffle-like transitions.
        //
        // This kind of transition that mixes conjunction and shuffle
        // would never appear in a naive implementation with only
        // conjunction and shuffle transitions, but no combinations.
        require(rank == 2,
                "infiltration: variadic product does not work");

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
        for (auto t: zip_map_tuple(out_(psrc)))
          // These are always new transitions: first because the
          // source state is visited for the first time, and second
          // because the couple (left destination, label) is unique,
          // and so is (right destination, label).
          if (!aut_->labelset()->is_one(t.first))
            detail::cross_tuple
              ([this,src,&t]
               (const typename transition_map_t<Auts>::transition&... ts)
               {
                 this->new_transition
                   (src,
                    static_if<ranked>
                    ([this](auto... s) { return state(s..., 0); },
                     [this](auto... s) { return state(s...); })
                    (ts.dst...),
                    t.first,
                    ws_.mul(ts.weight()...));
               },
               t.second);
        add_one_transitions_(src, psrc, aut_->indices);
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
        unsigned r = std::get<rank>(psrc);
        if (r <= I
            && have_proper_out(psrc, make_index_range<0, I>{}))
          {
            // one is guaranteed to be first.
            const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
            if (!tmap.empty() && ls.is_one(tmap.begin()->first))
              for (auto t : tmap.begin()->second)
                {
                  auto pdst = psrc;
                  std::get<I>(pdst) = t.dst;
                  static_if<ranked>([&pdst]{ std::get<rank>(pdst) = I; })();
                  this->new_transition(src, state(pdst), ls.one(), t.weight());
                }
          }
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
      template <bool Infiltration = false>
      void add_shuffle_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        weight_t final
          = add_shuffle_transitions_<Infiltration>(src, psrc, aut_->indices);
        aut_->set_final(src, final);
      }

      /// Let all automata advance one after the other, and add the
      /// corresponding transitions in the output.
      ///
      /// Return the product of the final states.
      template <bool Infiltration, size_t... I>
      weight_t add_shuffle_transitions_(const state_t src,
                                        const state_name_t& psrc,
                                        seq<I...>)
      {
        weight_t res = ws_.one();
        using swallow = int[];
        (void) swallow
        {
          (res = ws_.mul(res,
                         add_shuffle_transitions_<Infiltration, I>(src, psrc)),
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
      /// \tparam Infiltration
      ///    whether we are called after add_conjunction_transitions.
      /// \tparam I
      ///    the tape on which to perform a transition.
      template <bool Infiltration, size_t I>
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
            // case of infiltration), there may even exist such a
            // transition in the first loop.
            //
            // To trigger the later case, try the self-infiltration on
            // derived_term('a*a').
            for (auto d: t.second)
              {
                auto pdst = psrc;
                std::get<I>(pdst) = d.dst;
                if (Infiltration
                    || std::get<I>(psrc) == d.dst)
                  this->add_transition(src, state(pdst), t.first, d.weight());
                else
                  this->new_transition(src, state(pdst), t.first, d.weight());
              }
        return res;
      }
    };
  }

  /// A product automaton as a shared pointer.
  template <bool Lazy, Automaton Aut, Automaton... Auts>
  using product2_automaton
    = std::shared_ptr<detail::product2_automaton_impl<Lazy, Aut, Auts...>>;

  template <bool Lazy, Automaton Aut, Automaton... Auts>
  inline
  auto
  make_product2_automaton(Aut aut, const Auts&... auts)
    -> product2_automaton<Lazy, Aut, Auts...>
  {
    using res_t = product2_automaton<Lazy, Aut, Auts...>;
    return make_shared_ptr<res_t>(aut, auts...);
  }


  /*-----------------------------.
  | conjunction(automaton...).   |
  `-----------------------------*/

  /// Build the (accessible part of the) conjunction.
  template <Automaton... Auts>
  auto
  conjunction2(const Auts&... as)
  {
    auto res = make_product2_automaton<false>(meet_automata(as...),
                                      as...);
    res->conjunction();
    return res->strip();
  }

  /// Build the (accessible part of the) conjunction, on-the-fly.
  template <Automaton... Auts>
  auto
  conjunction2_lazy(const Auts&... as)
  {
    auto res = make_product2_automaton<true>(meet_automata(as...),
                                      as...);
    res->conjunction();
    return res;
  }

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
          return vcsn::conjunction2_lazy
             (as[I]->as<tuple_element_t<I, Auts>>()...);
        else
          return vcsn::conjunction2
             (as[I]->as<tuple_element_t<I, Auts>>()...);
      }

      /// Bridge (conjunction2).
      template <typename Auts, typename Bool>
      automaton
      conjunction2(const std::vector<automaton>& as, bool lazy)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return conjunction_<Auts>(as, lazy, indices);
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
  conjunction2(const Aut& aut, unsigned n)
    -> fresh_automaton_t_of<Aut>
  {
    // We used to compute `a & n` as `([^]* & a) & a)...`.  The code
    // was simpler, but the additional conjunction (with `[^]*`) made
    // it noticeably slower.  Concrete `a & 2` was twice slower than
    // `a & a`.
    auto res = make_fresh_automaton(aut);
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
          // a plein conjunction!
          res = strip(conjunction2(res, aut));
      }
    else
      {
        res = strip(conjunction2(aut, aut));
        n -= 2;
        static bool iterative = getenv("VCSN_ITERATIVE");
        if (iterative)
          for (size_t i = 0; i < n; ++i)
            res = strip(conjunction2(res, aut));
        else
          {
            auto power = strip(aut);
            while (true)
              {
                if (n % 2)
                  res = strip(conjunction2(res, power));
                n /= 2;
                if (!n)
                  break;
                power = strip(conjunction2(power, power));
              }
          }
      }

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction2).
      template <Automaton Aut, typename Unsigned>
      automaton
      conjunction_repeated2(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::conjunction2(a, n);
      }
    }
  }
}
