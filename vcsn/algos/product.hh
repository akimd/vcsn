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
# include <vcsn/misc/vector.hh>
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

  /// Join between automata in tuple.
  template <typename... Auts>
  mutable_automaton<join_t<context_t_of<Auts>...>>
  join(const std::tuple<Auts...>& auts)
  {
    auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
    return join_(auts, indices);
  }

  template <typename... Auts, size_t... I>
  mutable_automaton<join_t<context_t_of<Auts>...>>
  join_(const std::tuple<Auts...>& auts,
        vcsn::detail::index_sequence<I...>)
  {
    return join_automata(std::get<I>(auts)...);
  }

  /// Meet between automata in tuple.
  template <typename... Auts>
  auto
  meet(const std::tuple<Auts...>& auts)
    -> decltype(meet_(auts,
                      vcsn::detail::make_index_sequence<sizeof...(Auts)>{}))
  {
    auto indices = vcsn::detail::make_index_sequence<sizeof...(Auts)>{};
    return meet_(auts, indices);
  }

  template <typename... Auts, size_t... I>
  auto
  meet_(const std::tuple<Auts...>& auts,
        vcsn::detail::index_sequence<I...>)
    -> decltype(meet_automata(std::get<I>(auts)...))
  {
    return meet_automata(std::get<I>(auts)...);
  }

  namespace detail
  {
    /*---------------------------------.
    | product_automaton_impl<Aut...>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) product.
    template <typename... Auts>
    class product_automaton_impl
      : public tuple_automaton_impl<decltype(join_automata(std::declval<Auts>()...)),
                                    Auts...>
    {
      static_assert(all_<labelset_t_of<Auts>::is_letterized()...>(),
                    "requires letterized labels");

      /// The type of the resulting automaton.
      using automaton_t = decltype(join_automata(std::declval<Auts>()...));
      using super_t = tuple_automaton_impl<automaton_t, Auts...>;

    public:
      using typename super_t::pair_t;
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
        return "product_automaton" + super_t::sname_(false);
      }

      std::string vname(bool full = true) const
      {
        return "product_automaton" + super_t::vname_(full, false);
      }

      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltration.
      using context_t = join_t<context_t_of<Auts>...>;
      using labelset_t = labelset_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;

      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// The type of our transition maps: convert the weight to weightset_t,
      /// non deterministic, and including transitions to post().
      template <typename A>
      using transition_map_t = transition_map<A, weightset_t, false, true>;

    public:
      /// The type of input automata.
      using automata_t = std::tuple<Auts...>;

      /// The type of the Ith input automaton, unqualified.
      template <size_t I>
      using input_automaton_t
        = base_t<typename std::tuple_element<I, automata_t>::type>;

      using super_t::aut_;

      product_automaton_impl(const Auts&... aut)
        : super_t(join_automata(aut...), aut...)
        , transition_maps_{{aut, *aut_->weightset()}...}
      {}

      /// Compute the (accessible part of the) product.
      void product()
      {
        aut_ = meet(auts_);
        initialize_product();

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_product_transitions(src, psrc);
          }
      }

      /// Compute the (accessible part of the) shuffle product.
      void shuffle()
      {
        aut_ = join(auts_);
        initialize_shuffle();

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_shuffle_transitions(src, psrc);
          }
      }

      /// Compute the (accessible part of the) infiltration product.
      void infiltration()
      {
        aut_ = join(auts_);

        // Infiltrate is a mix of product and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for product:
        initialize_shuffle();

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
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

      /// The outgoing tuple of transitions from state tuple \a ss.
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const pair_t& ss)
      {
        return out_(ss, indices);
      }

      template <size_t... I>
      std::tuple<typename transition_map_t<Auts>::map_t&...>
      out_(const pair_t& ss, seq<I...>)
      {
        return std::tie(std::get<I>(transition_maps_)[std::get<I>(ss)]...);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_product_transitions(const state_t src,
                                   const pair_t& psrc)
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

      /// Add the epsilon transitions leaving the state src, if it is relevant
      /// (i.e. only for the labelsets that have epsilons).
      template <std::size_t... I>
      void
      add_one_transitions_(const state_t src, const pair_t& psrc, seq<I...>)
      {
        using swallow = int[];
        (void) swallow
        {
          (maybe_add_one_transitions_<I>(*(std::get<I>(auts_)->labelset()),
                                         src, psrc), 0)...
        };
      }

      /// In the case where the labelset doesn't have epsilon, do nothing
      template <std::size_t I, typename L>
      typename std::enable_if<!L::has_one(), void>::type
      maybe_add_one_transitions_(const L&, const state_t, const pair_t&)
      {}

      /// If the labelset has epsilon, add the relevant epsilon-transitions
      /// leaving the state
      template <std::size_t I, typename L>
      typename std::enable_if<L::has_one(), void>::type
      maybe_add_one_transitions_(const L& ls, const state_t src,
                                 const pair_t& psrc)
      {
        add_one_transitions_<I>(src, psrc,
                                std::get<I>(transition_maps_)
                                [std::get<I>(psrc)]
                                [ls.one()]);
      }

      /// If every tape after this one has only incoming epsilon transitions,
      /// add in the result automaton all the outgoing epsilon transitions of
      /// the Ith tape.
      template <std::size_t I>
      void
      add_one_transitions_(const state_t src, const pair_t& psrc,
                           const typename
                           transition_map_t<input_automaton_t<I>>::transitions_t&
                           epsilon_out)
      {
        if (!has_epsilon_in(psrc, I + 1, indices)
            && !has_only_epsilon_out(psrc, I, indices))
          for (auto t : epsilon_out)
            {
              auto pdst = psrc;
              std::get<I>(pdst) = t.dst;
              aut_->new_transition(src, state(pdst),
                                   aut_->labelset()->one(), t.wgt);
            }
      }

      /// Check if all the tapes after the Ith have only incoming epsilon
      /// transitions.
      template <std::size_t... I>
      bool has_epsilon_in(const pair_t& psrc, std::size_t i, seq<I...>) const
      {
        bool has_ones[] = { has_only_ones_in(std::get<I>(auts_),
                                             std::get<I>(psrc))... };
        for (; i < sizeof...(Auts); ++i)
          if (has_ones[i])
            return true;
        return false;
      }

      /// Check if all the tapes before the Ith have only outgoing epsilon
      /// transitions.
      template <std::size_t... I>
      bool has_only_epsilon_out(const pair_t& psrc, std::size_t i, seq<I...>)
      {
        bool has_ones[] = { has_only_ones_out(I, std::get<I>(auts_),
                                              std::get<I>(psrc))... };
        for (size_t j = 0; j < i; ++j)
          if (has_ones[j])
            return true;
        return false;
      }

      /// Check if the transition is epsilon (in the case of a labelset with
      /// epsilon).
      template <typename Aut_>
      typename std::enable_if<labelset_t_of<Aut_>::has_one(),
                              bool>::type
      is_one(const Aut_& aut, transition_t_of<Aut_> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      /// Same as above, but for labelsets without epsilon, so it's always
      /// false.
      template <typename Aut_>
      constexpr typename std::enable_if<!labelset_t_of<Aut_>::has_one(),
                              bool>::type
      is_one(const Aut_&, transition_t_of<Aut_>) const
      {
        return false;
      }

      /// Check if the state has only incoming epsilon transitions.
      /// As it is in the case of the epsilon-free labelset, it's always false.
      template <typename Aut_>
      constexpr typename std::enable_if<!labelset_t_of<Aut_>::has_one(),
                  bool>::type
      has_only_ones_in(const Aut_&,
                       state_t_of<Aut_>) const
      {
        return false;
      }

      /// Check if the state has only incoming epsilon transitions.
      template <typename Aut_>
      typename std::enable_if<labelset_t_of<Aut_>::has_one(),
                 bool>::type
      has_only_ones_in(const Aut_& rhs, state_t_of<Aut_> rst) const
      {
        auto rin = rhs->all_in(rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr) && !rhs->is_initial(rst);
      }

      /// Check if the state has only incoming epsilon transitions.
      template <typename Aut_>
      typename std::enable_if<labelset_t_of<Aut_>::has_one(),
                 bool>::type
      has_only_ones_out(size_t I, const Aut_& a, state_t_of<Aut_> st)
      {
        auto p = out_pair_t{I, st};
        auto lb = out_map.lower_bound(p);
        if (lb != out_map.end() && !out_map.key_comp()(p, lb->first))
          return lb->second;
        for (auto t : a->all_out(st))
          if (!is_one(a, t))
          {
            out_map.insert(lb, typename out_map_t::value_type(p, false));
            return false;
          }
        out_map.insert(lb, typename out_map_t::value_type(p, true));
        return true;
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given tuple of input state automata.  Update the worklist
      /// with the needed source-state pairs.
      void add_shuffle_transitions(const state_t src,
                                   const pair_t& psrc)
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
                                        const pair_t& psrc,
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
                               const pair_t& psrc)
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
      /// Map (automaton, state) -> has_only_epsilon_out
      using out_pair_t = std::pair<size_t, state_t>;
      using out_map_t = std::map<out_pair_t, bool>;
      out_map_t out_map;
    };
  }

  /// A product automaton as a shared pointer.
  template <typename... Auts>
  using product_automaton
    = std::shared_ptr<detail::product_automaton_impl<Auts...>>;

  template <typename... Auts>
  inline
  auto
  make_product_automaton(const Auts&... auts)
    -> product_automaton<Auts...>
  {
    using res_t = product_automaton<Auts...>;
    return make_shared_ptr<res_t>(auts...);
  }


  /*--------------------------------.
  | product(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) product.
  template <typename Lhs, typename Rhs>
  inline
  auto
  product(const Lhs& lhs, const Rhs& rhs)
    -> product_automaton<Lhs, Rhs>
  {
    auto res = make_product_automaton(lhs, rhs);
    res->product();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
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
    }
  }

  /*------------------------.
  | product(automaton...).  |
  `------------------------*/

  /// Build the (accessible part of the) product.
  template <typename... Auts>
  inline
  auto
  product(const Auts&... as)
    -> product_automaton<Auts...>
  {
    auto res = make_product_automaton(as...);
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

      /// Bridge.
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

  /*--------------------------------.
  | shuffle(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) shuffle.
  template <typename Lhs, typename Rhs>
  inline
  auto
  shuffle(const Lhs& lhs, const Rhs& rhs)
    -> product_automaton<Lhs, Rhs>
  {
    auto res = make_product_automaton(lhs, rhs);
    res->shuffle();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
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
    }
  }

  /*------------------------.
  | shuffle(automaton...).  |
  `------------------------*/

  /// Build the (accessible part of the) product.
  template <typename... Auts>
  inline
  auto
  shuffle(const Auts&... as)
    -> product_automaton<Auts...>
  {
    auto res = make_product_automaton(as...);
    res->shuffle();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename... Auts, size_t... I>
      automaton
      shuffle_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        auto res = vcsn::shuffle(as[I]->as<Auts>()...);
        return make_automaton(res);
      }

      /// Bridge.
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


  /*-------------------------------------.
  | infiltration(automaton, automaton).  |
  `-------------------------------------*/

  /// Build the (accessible part of the) infiltration.
  template <typename Lhs, typename Rhs>
  inline
  auto
  infiltration(const Lhs& lhs, const Rhs& rhs)
    -> product_automaton<Lhs, Rhs>
  {
    auto res = make_product_automaton(lhs, rhs);
    res->infiltration();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
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
  typename RatExpSet::ratexp_t
  conjunction(const RatExpSet& rs,
               const typename RatExpSet::ratexp_t& lhs,
               const typename RatExpSet::ratexp_t& rhs)
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
