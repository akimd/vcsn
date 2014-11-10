#ifndef VCSN_ALGOS_PRODUCT_HH
# define VCSN_ALGOS_PRODUCT_HH

# include <iostream>
# include <map>
# include <utility>

# include <vcsn/algos/insplit.hh>
# include <vcsn/algos/strip.hh>
# include <vcsn/core/tuple-automaton.hh>
# include <vcsn/core/transition-map.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
# include <vcsn/misc/vector.hh> // cross_tuple
# include <vcsn/misc/zip-maps.hh>

namespace vcsn
{
  /// Join between automata.
  template <typename... Auts>
  auto
  join_automata(Auts&&... auts)
    -> decltype(make_mutable_automaton(join(auts->context()...)))
  {
    return make_mutable_automaton(join(auts->context()...));
  }

  /// Meet between automata.
  template <typename... Auts>
  auto
  meet_automata(Auts&&... auts)
    -> decltype(make_mutable_automaton(meet(auts->context()...)))
  {
    return make_mutable_automaton(meet(auts->context()...));
  }

  namespace detail
  {
    /*---------------------------------.
    | product_automaton_impl<Aut...>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) product.
    template <typename Aut, typename... Auts>
    class product_automaton_impl
      : public tuple_automaton_impl<Aut, Auts...>
    {
      static_assert(all_<labelset_t_of<Auts>::is_letterized()...>(),
                    "product: requires letterized labels");

      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using super_t = tuple_automaton_impl<automaton_t, Auts...>;

    public:
      using typename super_t::state_name_t;
      using typename super_t::state_t;
      template <size_t... I>
      using seq = typename super_t::template seq<I...>;

      using super_t::auts_;
      using super_t::indices;
      using super_t::pmap_;
      using super_t::post_;
      using super_t::pre_;
      using super_t::todo_;

      using super_t::state;

      static std::string sname()
      {
        return "product_automaton" + super_t::sname_();
      }

      std::string vname(bool full) const
      {
        return "product_automaton" + super_t::vname_(full);
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
      using input_automaton_t
        = base_t<typename std::tuple_element<I, automata_t>::type>;

      using super_t::aut_;

      product_automaton_impl(Aut aut, const Auts&... auts)
        : super_t(aut, auts...)
        , transition_maps_{{auts, *aut_->weightset()}...}
      {}

      /// Compute the (accessible part of the) product.
      void product()
      {
        initialize_product();

        while (!todo_.empty())
          {
            state_name_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_product_transitions(src, psrc);
          }
      }

      /// Compute the (accessible part of the) shuffle product.
      void shuffle()
      {
        initialize_shuffle();

        while (!todo_.empty())
          {
            state_name_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_shuffle_transitions(src, psrc);
          }
      }

      /// Compute the (accessible part of the) infiltration product.
      void infiltration()
      {
        // Infiltrate is a mix of product and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for product:
        initialize_shuffle();

        while (!todo_.empty())
          {
            state_name_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            // Infiltrate is a mix of product and shuffle operations.
            //
            // Product transitions must be added before shuffle ones:
            // this way "product" can use "new_transition" only, which
            // is faster than "add_transition".
            add_product_transitions(src, psrc);
            add_shuffle_transitions(src, psrc);
          }
      }

    private:
      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_product()
      {
        todo_.emplace_back(pre_());
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle()
      {
        // Make the result automaton initial states: same as the
        // (synchronized) product of pre: synchronized transitions on $.
        add_product_transitions(aut_->pre(), pre_());
      }

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <typename A>
      using transition_map_t = transition_map<A, weightset_t, false, true>;

      /// The outgoing tuple of transitions from state tuple \a ss.
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const state_name_t& ss)
      {
        return out_(ss, indices);
      }

      template <size_t... I>
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const state_name_t& ss, seq<I...>)
      {
        return std::tie(std::get<I>(transition_maps_)[std::get<I>(ss)]...);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_product_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        for (auto t: zip_map_tuple(out_(psrc)))
          // These are always new transitions: first because the
          // source state is visited for the first time, and second
          // because the couple (left destination, label) is unique,
          // and so is (right destination, label).
          if (!aut_->labelset()->is_one(t.first))
            detail::cross_tuple
              ([&] (const typename transition_map_t<Auts>::transition&... ts)
               {
                 aut_->new_transition(src, state(ts.dst...),
                                      t.first, aut_->weightset()->mul(ts.wgt...));
               },
               t.second);
        add_one_transitions_(src, psrc, indices);
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
          (maybe_add_one_transitions_<I>(*(std::get<I>(auts_)->labelset()),
                                         src, psrc), 0)...
        };
      }

      /// In the case where the labelset doesn't have one, do nothing
      template <std::size_t I, typename L>
      typename std::enable_if<!L::has_one(), void>::type
      maybe_add_one_transitions_(const L&, const state_t, const state_name_t&)
      {}

      /// If the labelset has one, add the relevant
      /// spontaneous-transitions leaving the state
      template <std::size_t I, typename L>
      typename std::enable_if<L::has_one(), void>::type
      maybe_add_one_transitions_(const L& ls, const state_t src,
                                 const state_name_t& psrc)
      {
        if (!has_one_in(psrc, I + 1, indices)
            && !has_only_one_out(psrc, I, indices))
          {
            // one is guaranteed to be first.
            const auto& tmap = std::get<I>(transition_maps_)[std::get<I>(psrc)];
            if (ls.is_one(tmap.begin()->first))
              for (auto t : tmap.begin()->second)
                {
                  auto pdst = psrc;
                  std::get<I>(pdst) = t.dst;
                  aut_->new_transition(src, state(pdst), ls.one(), t.wgt);
                }
          }
      }

      /// Check if all the tapes after the Ith have only incoming
      /// spontaneous transitions.
      template <std::size_t... I>
      bool has_one_in(const state_name_t& psrc, std::size_t i,
                      seq<I...>) const
      {
        bool has_ones[] = { has_only_ones_in(std::get<I>(auts_),
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
      typename std::enable_if<labelset_t_of<Aut_>::has_one(),
                              bool>::type
      is_one(const Aut_& aut, transition_t_of<Aut_> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      /// Same as above, but for labelsets without one, so it's always
      /// false.
      template <typename Aut_>
      constexpr typename std::enable_if<!labelset_t_of<Aut_>::has_one(),
                              bool>::type
      is_one(const Aut_&, transition_t_of<Aut_>) const
      {
        return false;
      }

      /// Check if the state has only incoming spontaneous
      /// transitions.  As it is in the case of the one-free labelset,
      /// it's always false.
      template <typename Aut_>
      constexpr typename std::enable_if<!labelset_t_of<Aut_>::has_one(),
                  bool>::type
      has_only_ones_in(const Aut_&,
                       state_t_of<Aut_>) const
      {
        return false;
      }

      /// Whether the state has only incoming spontaneous transitions.
      template <typename Aut_>
      typename std::enable_if<labelset_t_of<Aut_>::has_one(),
                              bool>::type
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
          return std::get<I>(auts_)->labelset()->is_one(tmap.begin()->first);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given tuple of input state automata.  Update the worklist
      /// with the needed source-state pairs.
      void add_shuffle_transitions(const state_t src,
                                   const state_name_t& psrc)
      {
        weight_t final = add_shuffle_transitions_(src, psrc, indices);
        aut_->set_final(src, final);
      }

      /// Let all automata advance one after the other, and add the
      /// corresponding transitions in the output.
      ///
      /// Return the product of the final states.
      template <size_t... I>
      weight_t add_shuffle_transitions_(const state_t src,
                                        const state_name_t& psrc,
                                        seq<I...>)
      {
        weight_t res = aut_->weightset()->one();
        using swallow = int[];
        (void) swallow
        {
          (res = aut_->weightset()->mul(res,
                                        add_shuffle_transitions_<I>(src, psrc)),
           0)...
        };
        return res;
      }

      /// Let Ith automaton advance, and add the corresponding
      /// transitions in the output.
      ///
      /// If we reach a final state, return the corresponding final
      /// weight (zero otherwise).
      template <size_t I>
      weight_t
      add_shuffle_transitions_(const state_t src,
                               const state_name_t& psrc)
      {
        // Whether is a final state.
        weight_t res = aut_->weightset()->zero();

        auto& ts = std::get<I>(transition_maps_)[std::get<I>(psrc)];
        for (auto t: ts)
          if (std::get<I>(auts_)->labelset()->is_special(t.first))
            res = t.second.front().wgt;
          else
            // The src state is visited for the first time, so all
            // these transitions are new.  *Except* in the case where
            // we have a loop on some tapes.
            //
            // If add_product_transitions was called before (in the
            // case of infiltration), there may even exist such a
            // transition in the first loop.
            for (auto d: t.second)
              if (std::get<I>(psrc) == d.dst)
                aut_->add_transition(src, src, t.first, d.wgt);
              else
                {
                  auto pdst = psrc;
                  std::get<I>(pdst) = d.dst;
                  aut_->new_transition(src, state(pdst), t.first, d.wgt);
                }
        return res;
      }

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


  /*------------------------.
  | product(automaton...).  |
  `------------------------*/

  /// Build the (accessible part of the) product.
  template <typename... Auts>
  inline
  auto
  product(const Auts&... as)
    -> product_automaton<decltype(meet_automata(as...)),
                         Auts...>
  {
    auto res = make_product_automaton(meet_automata(as...),
                                      as...);
    res->product();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      template <std::size_t I, typename Aut>
      typename std::enable_if<labelset_t_of<Aut>::has_one()
                              && I != 0, Aut>::type
      do_insplit(Aut& aut)
      {
        return insplit(aut);
      }

      template <std::size_t I, typename Aut>
      typename std::enable_if<!labelset_t_of<Aut>::has_one()
                              || I == 0, Aut&>::type
      do_insplit(Aut& aut)
      {
        return aut;
      }

      template <typename... Auts, size_t... I>
      automaton
      product_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        return make_automaton(vcsn::product(do_insplit<I, Auts>(as[I]->as<Auts>())...));
      }

      /// Binary bridge.
      template <typename Lhs, typename Rhs>
      automaton
      product(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::product(l, r));
      }

      REGISTER_DECLARE(product,
                       (const automaton&, const automaton&) -> automaton);

      /// Variadic bridge.
      template <typename... Auts>
      automaton
      product_vector(const std::vector<automaton>& as)
      {
        auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
        return product_<Auts...>(as, indices);
      }

      REGISTER_DECLARE(product_vector,
                       (const std::vector<automaton>&) -> automaton);
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
    -> product_automaton<decltype(join_automata(as...)),
                         Auts...>
  {
    auto res = make_product_automaton(join_automata(as...), as...);
    res->shuffle();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Binary bridge.
      template <typename Lhs, typename Rhs>
      automaton
      shuffle(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::shuffle(l, r));
      }

      REGISTER_DECLARE(shuffle,
                       (const automaton&, const automaton&) -> automaton);

      /// Variadic bridge helper.
      template <typename... Auts, size_t... I>
      automaton
      shuffle_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        auto res = vcsn::shuffle(as[I]->as<Auts>()...);
        return make_automaton(res);
      }

      /// Variadic bridge.
      template <typename... Auts>
      automaton
      shuffle_vector(const std::vector<automaton>& as)
      {
        auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
        return shuffle_<Auts...>(as, indices);
      }

      REGISTER_DECLARE(shuffle_vector,
                       (const std::vector<automaton>&) -> automaton);
    }
  }


  /*--------------------------.
  | shuffle(ratexp, ratexp).  |
  `--------------------------*/

  /// Shuffle product of ratexps.
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
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      shuffle_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetRhs>();
        auto rs = join(l.ratexpset(), r.ratexpset());
        auto lr = rs.conv(l.ratexpset(), l.ratexp());
        auto rr = rs.conv(r.ratexpset(), r.ratexp());
        return make_ratexp(rs, ::vcsn::shuffle(rs, lr, rr));
      }

      REGISTER_DECLARE(shuffle_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }


  /*-----------------------------.
  | infiltration(automaton...).  |
  `-----------------------------*/

  /// The (accessible part of the) infiltration product.
  template <typename... Auts>
  inline
  auto
  infiltration(const Auts&... as)
    -> product_automaton<decltype(join_automata(as...)),
                         Auts...>
  {
    auto res = make_product_automaton(join_automata(as...), as...);
    res->infiltration();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Binary bridge.
      template <typename Lhs, typename Rhs>
      automaton
      infiltration(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::infiltration(l, r));
      }

      REGISTER_DECLARE(infiltration,
                       (const automaton&, const automaton&) -> automaton);

      /// Variadic bridge helper.
      template <typename... Auts, size_t... I>
      automaton
      infiltration_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        auto res = vcsn::infiltration(as[I]->as<Auts>()...);
        return make_automaton(res);
      }

      /// Variadic bridge.
      template <typename... Auts>
      automaton
      infiltration_vector(const std::vector<automaton>& as)
      {
        auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
        return infiltration_<Auts...>(as, indices);
      }

      REGISTER_DECLARE(infiltration_vector,
                       (const std::vector<automaton>&) -> automaton);
    }
  }

  /*----------------------.
  | power(automaton, n).  |
  `----------------------*/

  template <typename Aut>
  auto
  power(const Aut& aut, unsigned n)
    -> typename Aut::element_type::automaton_nocv_t
  {
    using res_t = typename Aut::element_type::automaton_nocv_t;
    auto res = make_shared_ptr<res_t>(aut->context());
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
            res = strip(product(res, aut));
        else
          {
            auto power = strip(aut);
            while (true)
              {
                if (n % 2)
                  res = strip(product(res, power));
                n /= 2;
                if (!n)
                  break;
                power = strip(product(power, power));
              }
          }
      }

    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Unsigned>
      automaton
      power(const automaton& aut, unsigned n)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::power(a, n));
      }

      REGISTER_DECLARE(power,
                       (const automaton&, unsigned) -> automaton);
    }
  }


  /*------------------------------.
  | conjunction(ratexp, ratexp).  |
  `------------------------------*/

  /// Intersection/Hadamard product of ratexps.
  template <typename RatExpSet>
  inline
  typename RatExpSet::value_t
  conjunction(const RatExpSet& rs,
               const typename RatExpSet::value_t& lhs,
               const typename RatExpSet::value_t& rhs)
  {
    return rs.conjunction(lhs, rhs);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      conjunction_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetRhs>();
        auto rs = join(l.ratexpset(), r.ratexpset());
        auto lr = rs.conv(l.ratexpset(), l.ratexp());
        auto rr = rs.conv(r.ratexpset(), r.ratexp());
        return make_ratexp(rs, ::vcsn::conjunction(rs, lr, rr));
      }

      REGISTER_DECLARE(conjunction_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }
}

#endif // !VCSN_ALGOS_PRODUCT_HH
