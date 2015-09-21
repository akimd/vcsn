#pragma once

#include <iostream>
#include <map>
#include <utility>

#include <vcsn/algos/insplit.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/core/join-automata.hh>
#include <vcsn/core/transition-map.hh>
#include <vcsn/core/tuple-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/expression.hh> // dyn::make_expression
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
    template <typename Aut, typename... Auts>
    class product_automaton_impl
      : public automaton_decorator<tuple_automaton<Aut, Auts...>>
    {
      static_assert(all_<labelset_t_of<Auts>::is_letterized()...>(),
                    "product: requires letterized labels");

      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using tuple_automaton_t = tuple_automaton<automaton_t, Auts...>;
      using self_t = product_automaton_impl;
      using super_t = automaton_decorator<tuple_automaton_t>;

    public:
      using state_name_t
        = typename tuple_automaton_t::element_type::state_name_t;
      using state_t
        = typename tuple_automaton_t::element_type::state_t;
      template <size_t... I>
      using seq
        = typename tuple_automaton_t::element_type::template seq<I...>;

      static symbol sname()
      {
        static symbol res("product_automaton"
                          + tuple_automaton_t::element_type::sname_());
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt) const
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

    public:
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
        : super_t{make_tuple_automaton(aut, auts...)}
        , transition_maps_{{auts, ws_}...}
      {}

      /// A map from result state to tuple of original states.
      auto origins() const
        -> decltype(aut_->origins())
      {
        return aut_->origins();
      }

      /// When performing the lazy construction, list of states that
      /// have been completed (i.e., their outgoing transitions have
      /// been computed).
      std::set<state_t> done_;

      /// Complete a state: find its outgoing transitions.
      void complete_(state_t s)
      {
        const auto& orig = origins();
        state_name_t sn = orig.at(s);
        add_conjunction_transitions(s, sn);
        done_.insert(s);
      }

      /// All the outgoing transitions.
      using super_t::all_out;
      auto all_out(state_t s)
        -> decltype(aut_->all_out(s))
      {
        if (!has(done_, s))
          complete_(s);
        return aut_->all_out(s);
      }

      /// All the outgoing transitions satisfying the predicate.
      template <typename Pred>
      auto all_out(state_t s, Pred pred)
        -> decltype(aut_->all_out(s, pred))
      {
        if (!has(done_, s))
          complete_(s);
        return aut_->all_out(s, pred);
      }

      // FIXME: clang workaround.
      struct label_equal_p
      {
        bool operator()(transition_t_of<self_t> t) const
        {
          return aut_.labelset()->equal(aut_.label_of(t), label_);
        }
        const self_t& aut_;
        // Capture by copy: in the case of the transpose_automaton, the
        // labels are transposed, so they are temporaries.
        label_t_of<self_t> label_;
      };

      // FIXME: clang workaround.
      struct not_to_post_p
      {
        bool operator()(transition_t_of<self_t> t) const
        {
          return aut_.dst_of(t) != aut_.post();
        }
        const self_t& aut_;
      };

      /// Indexes of visible transitions leaving state \a s.
      /// Invalidated by del_transition() and del_state().
      auto out(state_t s)
        -> decltype(this->all_out(s, not_to_post_p{*this}))
      {
        return all_out(s, not_to_post_p{*this});
      }

      /// Indexes of all transitions leaving state \a s on label \a l.
      /// Invalidated by del_transition() and del_state().
      auto out(state_t s, label_t_of<self_t> l)
        -> decltype(this->all_out(s, label_equal_p{*this, l}))
      {
        return all_out(s, label_equal_p{*this, l});
      }

      /// Compute the (accessible part of the) conjunction.
      void conjunction()
      {
        initialize_conjunction();

        while (!aut_->todo_.empty())
          {
            state_name_t psrc = aut_->todo_.front();
            aut_->todo_.pop_front();
            state_t src = aut_->pmap_[psrc];

            add_conjunction_transitions(src, psrc);
          }
      }

      /// Start the lazy computation of the (accessible part of the)
      /// conjunction.
      void conjunction_lazy()
      {
        initialize_conjunction();
      }

      /// Compute the (accessible part of the) shuffle product.
      void shuffle()
      {
        initialize_shuffle();

        while (!aut_->todo_.empty())
          {
            state_name_t psrc = aut_->todo_.front();
            aut_->todo_.pop_front();
            state_t src = aut_->pmap_[psrc];

            add_shuffle_transitions<false>(src, psrc);
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
        require(sizeof...(Auts) == 2,
                "infiltration: variadic product does not work");

        // Infiltrate is a mix of conjunction and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for conjunction:
        initialize_shuffle();

        while (!aut_->todo_.empty())
          {
            state_name_t psrc = aut_->todo_.front();
            aut_->todo_.pop_front();
            state_t src = aut_->pmap_[psrc];

            // Infiltrate is a mix of conjunction and shuffle operations.
            //
            // Conjunction transitions must be added before shuffle ones:
            // this way "conjunction" can use "new_transition" only, which
            // is faster than "add_transition".
            add_conjunction_transitions(src, psrc);
            add_shuffle_transitions<true>(src, psrc);
          }
      }

    private:
      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the conjunction algorithm.
      void initialize_conjunction()
      {
        aut_->todo_.emplace_back(aut_->pre_());
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle()
      {
        // Make the result automaton initial states: same as the
        // conjunction of pre: synchronized transitions on $.
        add_conjunction_transitions(aut_->pre(), aut_->pre_());
      }

      /// Conversion from state name to state number.
      template <typename... Args>
      state_t state(Args&&... args)
      {
        return aut_->state(std::forward<Args>(args)...);
      }

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <typename A>
      using transition_map_t = transition_map<A, weightset_t, false, true>;

      /// The outgoing tuple of transitions from state tuple \a ss.
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const state_name_t& ss)
      {
        return out_(ss, aut_->indices);
      }

      template <size_t... I>
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const state_name_t& ss, seq<I...>)
      {
        return std::tie(std::get<I>(transition_maps_)[std::get<I>(ss)]...);
      }

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
                 this->new_transition(src, state(ts.dst...),
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
          (maybe_add_one_transitions_<I>(*(std::get<I>(aut_->auts_)->labelset()),
                                         src, psrc), 0)...
        };
      }

      /// In the case where the labelset doesn't have one, do nothing
      template <std::size_t I, typename L>
      vcsn::enable_if_t<!L::has_one(), void>
      maybe_add_one_transitions_(const L&, const state_t, const state_name_t&)
      {}

      /// If the labelset has one, add the relevant
      /// spontaneous-transitions leaving the state
      template <std::size_t I, typename L>
      vcsn::enable_if_t<L::has_one(), void>
      maybe_add_one_transitions_(const L& ls, const state_t src,
                                 const state_name_t& psrc)
      {
        if (!has_one_in(psrc, I + 1, aut_->indices)
            && !has_only_one_out(psrc, I, aut_->indices))
          {
            // one is guaranteed to be first.
            const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
            if (ls.is_one(tmap.begin()->first))
              for (auto t : tmap.begin()->second)
                {
                  auto pdst = psrc;
                  std::get<I>(pdst) = t.dst;
                  this->new_transition(src, state(pdst), ls.one(), t.weight());
                }
          }
      }

      /// Check if all the tapes after the Ith have only incoming
      /// spontaneous transitions.
      template <std::size_t... I>
      bool has_one_in(const state_name_t& psrc, std::size_t i,
                      seq<I...>) const
      {
        bool has_ones[] = { has_only_ones_in(std::get<I>(aut_->auts_),
                                             std::get<I>(psrc))... };
        for (; i < sizeof...(Auts); ++i)
          if (has_ones[i])
            return true;
        return false;
      }

      /// Check if all the tapes before the Ith have only outgoing
      /// spontaneous transitions.
      template <std::size_t... I>
      bool has_only_one_out(const state_name_t& psrc, std::size_t i,
                            seq<I...>)
      {
        bool has_ones[] = { has_only_ones_out<I>(psrc)... };
        for (size_t j = 0; j < i; ++j)
          if (has_ones[j])
            return true;
        return false;
      }

      /// Check if the transition is spontaneous (in the case of a
      /// labelset with one).
      template <typename Aut_>
      vcsn::enable_if_t<labelset_t_of<Aut_>::has_one(), bool>
      is_one(const Aut_& aut, transition_t_of<Aut_> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      /// Same as above, but for labelsets without one, so it's always
      /// false.
      template <typename Aut_>
      constexpr vcsn::enable_if_t<!labelset_t_of<Aut_>::has_one(), bool>
      is_one(const Aut_&, transition_t_of<Aut_>) const
      {
        return false;
      }

      /// Check if the state has only incoming spontaneous
      /// transitions.  As it is in the case of the one-free labelset,
      /// it's always false.
      template <typename Aut_>
      constexpr vcsn::enable_if_t<!labelset_t_of<Aut_>::has_one(), bool>
      has_only_ones_in(const Aut_&,
                       state_t_of<Aut_>) const
      {
        return false;
      }

      /// Whether the state has only incoming spontaneous transitions.
      template <typename Aut_>
      vcsn::enable_if_t<labelset_t_of<Aut_>::has_one(), bool>
      has_only_ones_in(const Aut_& rhs, state_t_of<Aut_> rst) const
      {
        auto rin = rhs->all_in(rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr) && !rhs->is_initial(rst);
      }

      /// Whether the Ith state of \a psrc in the Ith input automaton
      /// has no non-spontaneous outgoing transitions.
      template <size_t I>
      bool
      has_only_ones_out(const state_name_t& psrc)
      {
        const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
        auto s = tmap.size();
        if (s == 0)
          return true;
        else if (2 <= s)
          return false;
        else
          return std::get<I>(aut_->auts_)->labelset()->is_one(tmap.begin()->first);
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

      /// The resulting weightset.
      const weightset_t& ws_ = *aut_->weightset();

      /// Transition caches.
      std::tuple<transition_map_t<Auts>...> transition_maps_;
    };
  }

  /// A product automaton as a shared pointer.
  template <typename Aut, typename... Auts>
  using product_automaton
    = std::shared_ptr<detail::product_automaton_impl<Aut, Auts...>>;

  template <typename Aut, typename... Auts>
  inline
  auto
  make_product_automaton(Aut aut, const Auts&... auts)
    -> product_automaton<Aut, Auts...>
  {
    using res_t = product_automaton<Aut, Auts...>;
    return make_shared_ptr<res_t>(aut, auts...);
  }


  /*-----------------------------.
  | conjunction(automaton...).   |
  `-----------------------------*/

  /// Build the (accessible part of the) conjunction.
  template <typename... Auts>
  inline
  auto
  conjunction_lazy(const Auts&... as)
    -> product_automaton<decltype(meet_automata(as...)),
                         Auts...>
  {
    auto res = make_product_automaton(meet_automata(as...),
                                      as...);
    res->conjunction_lazy();
    return res;
  }

  /// Build the (accessible part of the) conjunction.
  template <typename... Auts>
  inline
  auto
  conjunction(const Auts&... as)
    -> tuple_automaton<decltype(meet_automata(as...)),
                       Auts...>
  {
    auto res = make_product_automaton(meet_automata(as...),
                                      as...);
    res->conjunction();
    return res->strip();
  }

  namespace dyn
  {
    namespace detail
    {
      template <std::size_t I, typename Aut>
      vcsn::enable_if_t<labelset_t_of<Aut>::has_one() && I != 0, Aut>
      do_insplit(Aut& aut)
      {
        return insplit(aut);
      }

      template <std::size_t I, typename Aut>
      vcsn::enable_if_t<!labelset_t_of<Aut>::has_one() || I == 0, Aut&>
      do_insplit(Aut& aut)
      {
        return aut;
      }

      template <typename Auts, size_t... I>
      automaton
      conjunction_(const std::vector<automaton>& as,
                   vcsn::detail::index_sequence<I...>)
      {
        return make_automaton(vcsn::conjunction(do_insplit<I, std::tuple_element_t<I, Auts>>(as[I]->as<std::tuple_element_t<I, Auts>>())...));
      }

      template <typename Auts, size_t... I>
      automaton
      conjunction_lazy_(const std::vector<automaton>& as,
                        vcsn::detail::index_sequence<I...>)
      {
        return make_automaton(vcsn::conjunction_lazy(do_insplit<I, std::tuple_element_t<I, Auts>>(as[I]->as<std::tuple_element_t<I, Auts>>())...));
      }

      /// Bridge (conjunction).
      template <typename Auts, typename Bool>
      automaton
      conjunction_vector(const std::vector<automaton>& as,
                         bool lazy)
      {
        auto indices
          = vcsn::detail::make_index_sequence<std::tuple_size<Auts>::value>{};
        return (lazy
                ? conjunction_lazy_<Auts>(as, indices)
                : conjunction_<Auts>(as, indices));
      }
    }
  }


  /*------------------------.
  | shuffle(automaton...).  |
  `------------------------*/

  /// The (accessible part of the) shuffle product.
  template <typename... Auts>
  inline
  auto
  shuffle(const Auts&... as)
    -> tuple_automaton<decltype(join_automata(as...)),
                       Auts...>
  {
    auto res = make_product_automaton(join_automata(as...), as...);
    res->shuffle();
    return res->strip();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Variadic bridge helper.
      template <typename... Auts, size_t... I>
      automaton
      shuffle_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        auto res = vcsn::shuffle(as[I]->as<Auts>()...);
        return make_automaton(res);
      }

      /// Bridge (shuffle).
      template <typename... Auts>
      automaton
      shuffle_vector(const std::vector<automaton>& as)
      {
        auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
        return shuffle_<Auts...>(as, indices);
      }
    }
  }


  /*-----------------------------------.
  | shuffle(expression, expression).   |
  `-----------------------------------*/

  /// Shuffle product of expressions.
  template <typename ValueSet>
  inline
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
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return make_expression(rs, ::vcsn::shuffle(rs, lr, rr));
      }
    }
  }


  /*-----------------------------.
  | infiltration(automaton...).  |
  `-----------------------------*/

  /// The (accessible part of the) infiltration product.
  template <typename A1, typename A2>
  inline
  auto
  infiltration(const A1& a1, const A2& a2)
    -> tuple_automaton<decltype(join_automata(a1, a2)),
                       A1, A2>
  {
    auto res = make_product_automaton(join_automata(a1, a2), a1, a2);
    res->infiltration();
    return res->strip();
  }

  /// The (accessible part of the) infiltration product.
  template <typename A1, typename A2, typename A3, typename... Auts>
  inline
  auto
  infiltration(const A1& a1, const A2& a2, const A3& a3, const Auts&... as)
    -> decltype(infiltration(infiltration(a1, a2), a3, as...))
  {
    return infiltration(infiltration(a1, a2), a3, as...);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Variadic bridge helper.
      template <typename... Auts, size_t... I>
      automaton
      infiltration_(const std::vector<automaton>& as,
                    vcsn::detail::index_sequence<I...>)
      {
        auto res = vcsn::infiltration(as[I]->as<Auts>()...);
        return make_automaton(res);
      }

      /// Bridge (infiltration).
      template <typename... Auts>
      automaton
      infiltration_vector(const std::vector<automaton>& as)
      {
        auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
        return infiltration_<Auts...>(as, indices);
      }
    }
  }

  /*----------------------------------------.
  | infiltration(expression, expression).   |
  `----------------------------------------*/

  /// Infiltration product of expressions.
  template <typename ValueSet>
  inline
  typename ValueSet::value_t
  infiltration(const ValueSet& vs,
               const typename ValueSet::value_t& lhs,
               const typename ValueSet::value_t& rhs)
  {
    return vs.infiltration(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (infiltration).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      infiltration_expression(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return make_expression(rs, ::vcsn::infiltration(rs, lr, rr));
      }
    }
  }


  /*-----------------------------.
  | conjunction(automaton, n).   |
  `-----------------------------*/

  template <typename Aut>
  auto
  conjunction(const Aut& aut, unsigned n)
    -> fresh_automaton_t_of<Aut>
  {
    auto res = make_fresh_automaton(aut);
    {
      // automatonset::one().
      auto s = res->new_state();
      res->set_initial(s);
      res->set_final(s);
      for (auto l: res->context().labelset()->genset())
        res->new_transition(s, s, l);
    }

    if (n)
      {
        // FIXME: for 1, we should return the accessible part only.
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
      template <typename Aut, typename Unsigned>
      automaton
      conjunction_repeated(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::conjunction(a, n));
      }
    }
  }


  /*---------------------------------------.
  | conjunction(expression, expression).   |
  `---------------------------------------*/

  /// Intersection/Hadamard product of expressions.
  template <typename ExpSet>
  inline
  typename ExpSet::value_t
  conjunction(const ExpSet& rs,
              const typename ExpSet::value_t& lhs,
              const typename ExpSet::value_t& rhs)
  {
    return rs.conjunction(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (conjunction).
      template <typename ExpSetLhs, typename ExpSetRhs>
      expression
      conjunction_expression(const expression& lhs, const expression& rhs)
      {
        const auto& l = lhs->as<ExpSetLhs>();
        const auto& r = rhs->as<ExpSetRhs>();
        auto rs = join(l.expressionset(), r.expressionset());
        auto lr = rs.conv(l.expressionset(), l.expression());
        auto rr = rs.conv(r.expressionset(), r.expression());
        return make_expression(rs, ::vcsn::conjunction(rs, lr, rr));
      }
    }
  }
}
