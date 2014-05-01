#ifndef VCSN_ALGOS_PRODUCT_HH
# define VCSN_ALGOS_PRODUCT_HH

# include <deque>
# include <iostream>
# include <map>
# include <utility>

# include <vcsn/algos/copy.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
# include <vcsn/misc/tuple.hh>
# include <vcsn/misc/vector.hh>
# include <vcsn/misc/zip-maps.hh>

namespace vcsn
{

  namespace detail
  {

    /// Cache the outgoing transitions of an automaton as efficient
    /// maps label -> vector<(weight, dst)>.  Easy to zip.
    template <typename Aut, typename WeightSet = typename Aut::weightset_t>
    struct transition_map
    {
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      struct transition
      {
        /// The (converted) weight.
        weight_t wgt;
        typename Aut::state_t dst;
      };

      using map_t = std::map<typename Aut::label_t, std::vector<transition>>;
      std::map<typename Aut::state_t, map_t> maps_;

      transition_map(const Aut& aut, const weightset_t& ws)
        : aut_(aut)
        , ws_(ws)
      {}

      transition_map(const Aut& aut)
        : transition_map(aut, *aut.weightset())
      {}

      map_t& operator[](typename Aut::state_t s)
      {
        auto lb = maps_.lower_bound(s);
        if (lb == maps_.end() || maps_.key_comp()(s, lb->first))
          {
            // First insertion.
            lb = maps_.emplace_hint(lb, s, map_t{});
            auto& res = lb->second;
            for (auto t: aut_.all_out(s))
              {
                auto w = ws_.conv(*aut_.weightset(), aut_.weight_of(t));
                res[aut_.label_of(t)]
                  // FIXME: why do I have to call the ctor here?
                  .emplace_back(transition{w, aut_.dst_of(t)});
              }
          }
        return lb->second;
      }
      /// The automaton whose transitions are cached.
      const Aut& aut_;
      /// The result weightset.
      const weightset_t& ws_;
    };


    /*----------------------------------.
    | producter<automaton, automaton>.  |
    `----------------------------------*/

    /// Build the (accessible part of the) product.
    template <typename... Auts>
    class producter
    {
//static_assert(Auts::context_t::is_lal,
//"requires labels_are_letters")...;

      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltration.
      using context_t = join_t<typename Auts::context_t...>;
      using labelset_t = typename context_t::labelset_t;
      using weightset_t = typename context_t::weightset_t;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;
      /// The list of automaton indices as a static list.
      using indices_t = vcsn::detail::make_index_sequence<sizeof...(Auts)>;

    public:
      /// The type of input automata.
      using automata_t = std::tuple<const Auts&...>;
      /// The type of the resulting automaton.
      using automaton_t = mutable_automaton<context_t>;

      /// The type of the Ith input automaton, unqualified.
      template <size_t I>
      using input_automaton_t
        = typename std::remove_cv<
            typename std::remove_reference<
              typename std::tuple_element<I, automata_t>
              ::type>
            ::type>
        ::type;

      producter(const Auts&... aut)
        : auts_(aut...)
        , res_(join(aut.context()...))
        , transition_maps_{{aut, *res_.weightset()}...}
      {}

      /// Reset the attributes before a new product.
      void clear()
      {
        pmap_.clear();
        todo_.clear();
      }

      /// The (accessible part of the) product of \a lhs_ and \a rhs_.
      automaton_t product()
      {
        auto ctx = meet_();
        const auto& ws = *ctx.weightset();
        res_ = std::move(automaton_t(ctx));

        initialize_product();

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_product_transitions(ws, src, psrc);
          }
        return std::move(res_);
      }

      /// The (accessible part of the) shuffle product of \a lhs_ and
      /// \a rhs_.
      automaton_t shuffle()
      {
        auto ctx = join_();
        const auto& ws = *ctx.weightset();
        res_ = automaton_t(ctx);

        initialize_shuffle(ws);

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_shuffle_transitions(ws, src, psrc);
          }
        return std::move(res_);
      }

      /// The (accessible part of the) infiltration product of \a
      /// lhs_ and \a rhs_.
      automaton_t infiltration()
      {
        auto ctx = join_();
        const auto& ws = *ctx.weightset();
        res_ = automaton_t(ctx);

        // Infiltrate is a mix of product and shuffle operations, and
        // the initial states for shuffle are a superset of the
        // initial states for product:
        initialize_shuffle(ws);

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
            add_product_transitions(ws, src, psrc);
            add_shuffle_transitions(ws, src, psrc);
          }

        return std::move(res_);
      }

      /// Result state type.
      using state_t = typename automaton_t::state_t;
      /// Tuple of states of input automata.
      using pair_t = std::tuple<typename Auts::state_t...>;
      /// A map from result state to tuple of original states.
      using origins_t = std::map<state_t, pair_t>;

      /// A map from result state to tuple of original states.
      origins_t origins() const
      {
        origins_t res;
        for (const auto& p: pmap_)
          res.emplace(p.second, p.first);
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins.\n"
             "    node [shape = box, style = rounded]\n";
        for (auto p: orig)
          if (p.first != automaton_t::pre() && p.first != automaton_t::post())
            {
              o << "    " << p.first - 2
                << " [label = \"";
              const char* sep = "";
              for_(p.second,
                   [&](unsigned s) { o << sep << s - 2; sep = ","; });
              o << "\"]\n";
            }
        o << "*/\n";
        return o;
      }

    private:
      /// The join of the contexts.
      context_t join_() const
      {
        return join_(indices_t{});
      }

      template <size_t... I>
      context_t join_(seq<I...>) const
      {
        return join((std::get<I>(auts_).context())...);
      }

      /// The meet of the contexts.
      context_t meet_() const
      {
        return meet_(indices_t{});
      }

      template <size_t... I>
      context_t meet_(seq<I...>) const
      {
        return meet((std::get<I>(auts_).context())...);
      }

      /// The pre of the input automata.
      pair_t pre_() const
      {
        return pre_(indices_t{});
      }

      template <size_t... I>
      pair_t pre_(seq<I...>) const
      {
        // clang 3.4 on top of libstdc++ wants this ctor to be
        // explicitly called.
        return pair_t{(std::get<I>(auts_).pre())...};
      }

      /// The post of the input automata.
      pair_t post_() const
      {
        return post_(indices_t{});
      }

      template <size_t... I>
      pair_t post_(seq<I...>) const
      {
        // clang 3.4 on top of libstdc++ wants this ctor to be
        // explicitly called.
        return pair_t{(std::get<I>(auts_).post())...};
      }


      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// Input automata, supplied at construction time.
      automata_t auts_;

      /// Map state-tuple -> result-state.
      using map = std::map<pair_t, state_t>;
      map pmap_;

      /// Worklist of state tuples.
      std::deque<pair_t> todo_;

      /// Add the pre and post states in the result automaton.  This
      /// is needed for all three algorithms here.
      void initialize()
      {
        pmap_[pre_()] = res_.pre();
        pmap_[post_()] = res_.post();
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_product()
      {
        initialize();
        todo_.emplace_back(pre_());
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the shuffle algorithm.
      void initialize_shuffle(const weightset_t& ws)
      {
        initialize();
        // Make the result automaton initial states: same as the
        // (synchronized) product of pre: synchronized transitions on $.
        add_product_transitions(ws, res_.pre(), pre_());
      }

      /// The state in the product corresponding to a pair of states
      /// of operands.
      ///
      /// Add the given two source-automaton states to the worklist
      /// for the given result automaton if they aren't already there,
      /// updating the map; in any case return.
      state_t state(typename Auts::state_t... ss)
      {
        pair_t state{ss...};
        auto lb = pmap_.lower_bound(state);
        if (lb == pmap_.end() || pmap_.key_comp()(state, lb->first))
          {
            lb = pmap_.emplace_hint(lb, state, res_.new_state());
            todo_.emplace_back(state);
          }
        return lb->second;
      }

      /// The outgoing tuple of transitions from state tuple \a ss.
      std::tuple<typename transition_map<Auts, weightset_t>::map_t&...>
      out_(const pair_t& ss)
      {
        return out_(ss, indices_t{});
      }

      template <size_t... I>
      std::tuple<typename transition_map<Auts, weightset_t>::map_t&...>
      out_(const pair_t& ss, seq<I...>)
      {
        return std::tie(std::get<I>(transition_maps_)[std::get<I>(ss)]...);
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_product_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
      {
        for (auto t: zip_map_tuple(out_(psrc)))
          // These are always new transitions: first because the
          // source state is visited for the first time, and second
          // because the couple (left destination, label) is unique,
          // and so is (right destination, label).
          {
//            SHOWH(V(src)
//                 << V(std::get<0>(t.second).dst)
//                 << V(std::get<1>(t.second).dst)
//                 << V(t.first)
//                 << V(std::get<0>(t.second).wgt)
//                 << V(std::get<1>(t.second).wgt));
            detail::cross_tuple
              ([&] (const typename transition_map<Auts, weightset_t>::transition&... ts)
               {
                 res_.new_transition(src, state(ts.dst...),
                                     t.first, ws.mul(ts.wgt...));
               },
               t.second);
          }
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given tuple of input state automata.  Update the worklist
      /// with the needed source-state pairs.
      void add_shuffle_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
      {
        auto lsrc = std::get<0>(psrc);
        auto rsrc = std::get<1>(psrc);

        /// The weight as a final state.
        auto w = ws.one();

        // The src state is visited for the first time, so all these
        // transitions are new.  *Except* in the case where we have a
        // loop on both the lhs, and the rhs.
        //
        // If add_product_transitions was called before (in the case
        // of infiltration), there may even exist such a transition in
        // the first loop.
        {
          bool final = false;
          auto& ts = std::get<0>(transition_maps_)[lsrc];
          for (auto t: ts)
            if (std::get<0>(auts_).labelset()->is_special(t.first))
              {
                w = ws.mul(w, t.second.front().wgt);
                final = true;
              }
            else
              for (auto d: t.second)
                {
                  auto ldst = d.dst;
                  if (lsrc == ldst)
                    res_.add_transition(src, state(ldst, rsrc), t.first, d.wgt);
                  else
                    res_.new_transition(src, state(ldst, rsrc), t.first, d.wgt);
                }
          if (!final)
            w = ws.zero();
        }

        {
          bool final = false;
          auto& ts = std::get<1>(transition_maps_)[rsrc];
          for (auto t: ts)
            if (std::get<1>(auts_).labelset()->is_special(t.first))
              {
                w = ws.mul(w, t.second.front().wgt);
                final = true;
              }
            else
              for (auto d: t.second)
                {
                  auto rdst = d.dst;
                  if (rsrc == rdst)
                    res_.add_transition(src, state(lsrc, rdst), t.first, d.wgt);
                  else
                    res_.new_transition(src, state(lsrc, rdst), t.first, d.wgt);
                }
          if (!final)
            w = ws.zero();
        }

        res_.set_final(src, w);
      }

      /// The computed product.
      automaton_t res_;
      std::tuple<transition_map<Auts, weightset_t>...> transition_maps_;
    };
  }

  /*--------------------------------.
  | product(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) product.
  template <typename Lhs, typename Rhs>
  auto
  product(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::producter<Lhs, Rhs>::automaton_t
  {
    detail::producter<Lhs, Rhs> product(lhs, rhs);
    auto res = product.product();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
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
        return make_automaton(product(l, r));
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
  auto
  product(const Auts&... as)
    -> typename detail::producter<Auts...>::automaton_t
  {
    detail::producter<Auts...> product(as...);
    auto res = product.product();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      template <typename... Auts, size_t... I>
      automaton
      product_(const std::vector<automaton>& as,
               vcsn::detail::index_sequence<I...>)
      {
        return make_automaton(product(as[I]->as<Auts>()...));
      }


      /// Bridge.
      template <typename... Auts>
      automaton
      product_vector(const std::vector<automaton>& as)
      {
        return product_<Auts...>(as,
                        vcsn::detail::make_index_sequence<sizeof...(Auts)>{});
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
  auto
  shuffle(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::producter<Lhs, Rhs>::automaton_t
  {
    detail::producter<Lhs, Rhs> product(lhs, rhs);
    auto res = product.shuffle();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
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
        return make_automaton(shuffle(l, r));
      }

      REGISTER_DECLARE(shuffle,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*-------------------------------------.
  | infiltration(automaton, automaton).  |
  `-------------------------------------*/

  /// Build the (accessible part of the) infiltration.
  template <typename Lhs, typename Rhs>
  auto
  infiltration(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::producter<Lhs, Rhs>::automaton_t
  {
    detail::producter<Lhs, Rhs> product(lhs, rhs);
    auto res = product.infiltration();
    if (getenv("VCSN_ORIGINS"))
      product.print(std::cout, product.origins());
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
        return make_automaton(infiltration(l, r));
      }

      REGISTER_DECLARE(infiltration,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

  /*----------------------.
  | power(automaton, n).  |
  `----------------------*/

  template <typename Aut>
  Aut
  power(const Aut& aut, unsigned n)
  {
    Aut res(aut.context());
    {
      // automatonset::one().
      auto s = res.new_state();
      res.set_initial(s);
      res.set_final(s);
      for (auto l: res.context().labelset()->genset())
        res.new_transition(s, s, l);
    }

    if (n)
      {
        // FIXME: for 1, we should return the accessible part only.
        static bool iterative = getenv("VCSN_ITERATIVE");
        if (iterative)
          for (size_t i = 0; i < n; ++i)
            res = std::move(product(res, aut));
        else
          {
            Aut power = copy(aut);
            while (true)
              {
                if (n % 2)
                  res = std::move(product(res, power));
                n /= 2;
                if (!n)
                  break;
                power = std::move(product(power, power));
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
        return make_automaton(power(a, n));
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
        return make_ratexp(rs, conjunction(rs, lr, rr));
      }

      REGISTER_DECLARE(conjunction_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }
}

#endif // !VCSN_ALGOS_PRODUCT_HH
