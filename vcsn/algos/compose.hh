#ifndef VCSN_ALGOS_COMPOSE_HH
# define VCSN_ALGOS_COMPOSE_HH

# include <deque>
# include <iostream>
# include <map>

# include <vcsn/algos/insplit.hh>
# include <vcsn/algos/sort.hh>
# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/labelset/tupleset.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/tuple.hh>

namespace vcsn
{

  namespace detail
  {
    template <typename A, typename I>
    struct hidden_label_type;

    template <typename Aut, std::size_t... I>
    struct hidden_label_type<Aut, index_sequence<I...>>
    {
      template <std::size_t J>
      using elem = typename std::tuple_element<J,
            typename labelset_t_of<Aut>::valuesets_t>::type;

      using type = tupleset<elem<I>...>;
    };

    /// Read-write on an automaton, that hides all bands but one.
    template <std::size_t Band, typename Aut>
    class blind_automaton_impl : public automaton_decorator<Aut>
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;
      using super = automaton_decorator<Aut>;

      static_assert(context_t_of<Aut>::is_lat, "requires labels_are_tuples");
      static_assert(Band < labelset_t_of<Aut>::size(),
                    "band outside of the tuple");

      /// The type of the automata to produce from this kind o
      /// automata.  For instance, insplitting on a
      /// blind_automaton<const mutable_automaton<Ctx>> should
      /// yield a blind_automaton<mutable_automaton<Ctx>>, without
      /// the "inner" const.
      using automaton_nocv_t
        = blind_automaton<Band,
                          typename automaton_t::element_type::automaton_nocv_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      // Exposed label
      using label_t
        = typename std::tuple_element<Band, label_t_of<automaton_t>>::type;
      // Underlying automaton label
      using hidden_label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using hidden_indices_t = concat_sequence<
                               typename make_index_range<0, Band>::type,
                               typename make_index_range<Band + 1,
                                 std::tuple_size<hidden_label_t>::value
                                   - Band - 1>::type>;

      using labelset_t
        = typename std::tuple_element<Band,
                                      typename labelset_t_of<automaton_t>::valuesets_t>::type;
      using hidden_labelset_t = labelset_t_of<automaton_t>;

      // All bands except the exposed one
      using res_labelset_t = typename hidden_label_type<Aut, hidden_indices_t>::type;
      using res_label_t = typename res_labelset_t::value_t;
      using weightset_t = weightset_t_of<automaton_t>;

      using labelset_ptr = std::shared_ptr<const labelset_t>;
      using context_t = ::vcsn::context<res_labelset_t, weightset_t>;

      using weightset_ptr = typename automaton_t::element_type::weightset_ptr;

    public:
      using super::super;

      blind_automaton_impl(const context_t_of<automaton_t>& ctx)
        : blind_automaton_impl(std::make_shared<typename automaton_t::element_type>(ctx))
      {}

      static std::string sname()
      {
        return "blind_automaton<" + automaton_t::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "blind_automaton<" + this->aut_->vname(full) + ">";
      }

      res_label_t
      hidden_label_of(transition_t t) const
      {
        return hidden_label_of_(t, hidden_indices);
      }

      res_label_t
      hidden_one() const
      {
        return hidden_one_<hidden_labelset_t>(hidden_indices);
      }

      res_labelset_t
      res_labelset() const
      {
        return res_labelset_(hidden_indices);
      }

      std::shared_ptr<labelset_t>
      labelset() const
      {
        return std::make_shared<labelset_t>(std::get<Band>(this->aut_->labelset()->sets()));
      }

    private:
      hidden_indices_t hidden_indices{};

      static label_t hide_(hidden_label_t l)
      {
        return std::get<Band>(l);
      }

      template <std::size_t... I>
      res_label_t hidden_label_of_(transition_t t, index_sequence<I...>) const
      {
        hidden_label_t l = this->aut_->label_of(t);
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      typename std::enable_if<L::has_one(), res_label_t>::type
      hidden_one_(index_sequence<I...>) const
      {
        hidden_label_t l = this->aut_->labelset()->one();
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      typename std::enable_if<!L::has_one(), res_label_t>::type
      hidden_one_(index_sequence<I...>) const
      {
        raise("Should not get here");
      }

      template <std::size_t... I>
      res_labelset_t res_labelset_(index_sequence<I...>) const
      {
        return res_labelset_t{std::get<I>(this->aut_->labelset()->sets())...};
      }

    public:

      /*----------------------------.
      | const methods that change.  |
      `----------------------------*/

      auto label_of(transition_t t) const
        -> decltype(hide_(this->aut_->label_of(t)))
      {
        return hide_(this->aut_->label_of(t));
      }

      template <typename A>
      transition_t new_transition_copy(const A& aut, state_t src,
                                       state_t dst, transition_t t, weight_t k)
      {
        return this->aut_->new_transition_copy(aut->original_automaton(),
                                               src, dst, t, k);
      }

      template <typename A>
      weight_t add_transition_copy(const A& aut, state_t src,
                                   state_t dst, transition_t t, weight_t k)
      {
        return this->aut_->add_transition_copy(aut->original_automaton(),
                                               src, dst, t, k);
      }
    };


    template <typename T1, typename T2>
    struct concat_tupleset;

    template <typename... T1, typename... T2>
    struct concat_tupleset<tupleset<T1...>, tupleset<T2...>>
    {
      using type = tupleset<T1..., T2...>;
    };


    /*---------------------------------.
    | composer<automaton, automaton>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) composition.
    template <typename Lhs, typename Rhs>
    class composer
    {
      static_assert(context_t_of<Lhs>::is_lat,
                    "requires labels_are_tuples");
      static_assert(context_t_of<Rhs>::is_lat,
                    "requires labels_are_tuples");

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

    public:
      using clhs_t = Lhs;
      using crhs_t = Rhs;
      using hidden_l_label_t = typename clhs_t::element_type::res_label_t;
      using hidden_r_label_t = typename crhs_t::element_type::res_label_t;
      using hidden_l_labelset_t = typename clhs_t::element_type::res_labelset_t;
      using hidden_r_labelset_t = typename crhs_t::element_type::res_labelset_t;

      static_assert(std::is_same<labelset_t_of<clhs_t>,
                    labelset_t_of<crhs_t>>::value,
                    "common band must be of same type");
      using middle_labelset_t = labelset_t_of<clhs_t>;
      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltration.
      using weightset_t = join_t<weightset_t_of<context_t_of<Lhs>>,
                                 weightset_t_of<context_t_of<Rhs>>>;
      using labelset_t = typename concat_tupleset<hidden_l_labelset_t,
                                                  hidden_r_labelset_t>::type;

      using res_label_t = typename labelset_t::value_t;
      using context_t = ::vcsn::context<labelset_t, weightset_t>;

      /// The type of the resulting automaton.
      using automaton_t = mutable_automaton<context_t>;

      /// Result state type.
      using state_t = state_t_of<automaton_t>;
      /// Tuple of states of input automata.
      using pair_t = std::pair<state_t_of<Lhs>, state_t_of<Rhs>>;
      /// A map from result state to tuple of original states.
      using origins_t = std::map<state_t, pair_t>;

      composer(const Lhs& lhs, const Rhs& rhs)
        : lhs_(lhs)
        , rhs_(rhs)
        , res_(std::make_shared<typename automaton_t::element_type>(context_t{make_labelset_(lhs_->res_labelset(), rhs_->res_labelset()),
                join(*lhs->weightset(), *rhs->weightset())}))
      {}

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

      /// Reset the attributes before a new product.
      void clear()
      {
        pmap_.clear();
        todo_.clear();
      }

      /// The (accessible part of the) product of \a lhs_ and \a rhs_.
      automaton_t compose()
      {
        initialize_compose();
        const auto& ws = *res_->context().weightset();

        while (!todo_.empty())
          {
            pair_t psrc = todo_.front();
            todo_.pop_front();
            state_t src = pmap_[psrc];

            add_compose_transitions(ws, src, psrc);
          }
        return std::move(res_);
      }


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
          if (p.first != automaton_t::element_type::pre()
              && p.first != automaton_t::element_type::post())
            {
              o << "    " << p.first - 2
                << " [label = \"";
              o << p.second.first - 2 << "," << p.second.second - 2;
              o << "\"]\n";
            }
        o << "*/\n";
        return o;
      }

    private:

      /// The pre of the input automata.
      pair_t pre_() const
      {
        return pair_t{lhs_->pre(), rhs_->pre()};
      }

      /// The post of the input automata.
      pair_t post_() const
      {
        return pair_t{lhs_->post(), rhs_->post()};
      }

      /// Map state-tuple -> result-state.
      using map = std::map<pair_t, state_t>;
      map pmap_;

      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// Input automata, supplied at construction time.
      clhs_t lhs_;
      crhs_t rhs_;

      /// Worklist of state tuples.
      std::deque<pair_t> todo_;

      /// Add the pre and post states in the result automaton.  This
      /// is needed for all three algorithms here.
      void initialize()
      {
        pmap_[pre_()] = res_->pre();
        pmap_[post_()] = res_->post();
      }

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_compose()
      {
        initialize();
        todo_.emplace_back(pre_());
      }

      /// The state in the product corresponding to a pair of states
      /// of operands.
      ///
      /// Add the given two source-automaton states to the worklist
      /// for the given result automaton if they aren't already there,
      /// updating the map; in any case return.
      state_t state(state_t_of<Lhs> ls, state_t_of<Rhs> rs)
      {
        pair_t state{ls, rs};
        auto lb = pmap_.lower_bound(state);
        if (lb == pmap_.end() || pmap_.key_comp()(state, lb->first))
          {
            lb = pmap_.emplace_hint(lb, state, res_->new_state());
            todo_.emplace_back(state);
          }
        return lb->second;
      }

      res_label_t join_label(hidden_l_label_t ll, hidden_r_label_t rl)
      {
        return std::tuple_cat(ll, rl);
      }

      template<typename Aut>
      typename std::enable_if<labelset_t_of<Aut>::has_one(),
                              typename Aut::element_type::res_label_t>::type
      get_hidden_one(const Aut& aut)
      {
        return aut->hidden_one();
      }

      template<typename Aut>
      typename std::enable_if<!labelset_t_of<Aut>::has_one(),
                              typename Aut::element_type::res_label_t>::type
      get_hidden_one(const Aut&)
      {
        raise("should not get here");
      }

      /// Add transitions to the given result automaton, starting from
      /// the given result input state, which must correspond to the
      /// given pair of input state automata.  Update the worklist with
      /// the needed source-state pairs.
      void add_compose_transitions(const weightset_t& ws,
                                   const state_t src,
                                   const pair_t& psrc)
      {
        // This relies on outgoing transitions being sorted by label
        // by the sort algorithm: thanks to that property we can scan
        // the two successor lists in lockstep. Thus if there is a one
        // transition, it is at the beginning.
        auto ls = lhs_->all_out(psrc.first);
        auto rs = rhs_->all_out(psrc.second);
        auto li = ls.begin();
        auto ri = rs.begin();

        for (/* Nothing. */; li != ls.end() && is_one(lhs_, *li); ++li)
          if (!has_only_ones_in(rhs_, psrc.second))
            res_->new_transition(src, state(lhs_->dst_of(*li), psrc.second),
                                join_label(lhs_->hidden_label_of(*li),
                                           get_hidden_one(rhs_)),
                                ws.mul(ws.conv(*lhs_->weightset(),
                                               lhs_->weight_of(*li)),
                                       ws.conv(*rhs_->weightset(),
                                               rhs_->context().weightset()->one())));

        for (/* Nothing. */; ri != rs.end() && is_one(rhs_, *ri); ++ri)
          res_->new_transition(src, state(psrc.first, rhs_->dst_of(*ri)),
                              join_label(get_hidden_one(lhs_),
                                         rhs_->hidden_label_of(*ri)),
                              ws.mul(ws.conv(*lhs_->weightset(),
                                             lhs_->context().weightset()->one()),
                                     ws.conv(*rhs_->weightset(),
                                             rhs_->weight_of(*ri))));


        for (/* Nothing. */;
             li != ls.end() && ri != rs.end();
             ++ li)
        {
          auto lt = *li;
          label_t_of<clhs_t> label = lhs_->label_of(lt);
          // Skip right-hand transitions with labels we don't have
          // on the left hand.
          while (middle_labelset_t::less_than(rhs_->label_of(*ri), label))
            if (++ ri == rs.end())
              return;

          // If the smallest label on the right-hand side is bigger
          // than the left-hand one, we have no hope of ever adding
          // transitions with this label.
          if (middle_labelset_t::less_than(label, rhs_->label_of(*ri)))
            continue;

          assert(middle_labelset_t::equals(label, rhs_->label_of(*ri)));
          auto rstart = ri;
          while (middle_labelset_t::equals(rhs_->label_of(*ri), label))
          {
            // These are always new transitions: first because the
            // source state is visited for the first time, and
            // second because the couple (left destination, label)
            // is unique, and so is (right destination, label).
            res_->new_transition(src, state(lhs_->dst_of(lt), rhs_->dst_of(*ri)),
                                join_label(lhs_->hidden_label_of(*li),
                                           rhs_->hidden_label_of(*ri)),
                                ws.mul(ws.conv(*lhs_->weightset(),
                                               lhs_->weight_of(lt)),
                                       ws.conv(*rhs_->weightset(),
                                               rhs_->weight_of(*ri))));

            if (++ ri == rs.end())
              break;
          }

          // Move the right-hand iterator back to the beginning of
          // the matching part.  This will be needed if the next
          // left-hand transition has the same label.
          ri = rstart;
        }
      }

      template <typename A>
      typename std::enable_if<labelset_t_of<A>::has_one(),
                              bool>::type
      is_one(const A& aut, transition_t_of<A> tr) const
      {
        return aut->labelset()->is_one(aut->label_of(tr));
      }

      template <typename A>
      constexpr
      typename std::enable_if<!labelset_t_of<A>::has_one(),
                              bool>::type
      is_one(const A&, transition_t_of<A>)
      const
      {
        return false;
      }

      template <typename Aut>
      constexpr
      typename std::enable_if<!labelset_t_of<Aut>::has_one(),
                              bool>::type
      has_only_ones_in(const Aut&, state_t_of<Aut>) const
      {
        return false;
      }

      template <typename Aut>
      typename std::enable_if<labelset_t_of<Aut>::has_one(),
                              bool>::type
      has_only_ones_in(const Aut& rhs, state_t_of<Aut> rst) const
      {
        auto rin = rhs->all_in(rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr) && !rhs->is_initial(rst);
      }


      /// The computed product.
      automaton_t res_;
    };

    template <std::size_t Band, typename AutPtr>
    using blind_automaton
      = std::shared_ptr<detail::blind_automaton_impl<Band, AutPtr>>;

    template <std::size_t Band, typename Aut>
    blind_automaton<Band, Aut>
    make_blind_automaton(Aut& aut)
    {
      return std::make_shared<detail::blind_automaton_impl<Band, Aut>>(aut);
    }

    template<typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::has_one(),
                            detail::blind_automaton<0, Aut>>::type
    get_insplit(Aut& aut)
    {
      return insplit(make_blind_automaton<0>(aut));
    }

    template<typename Aut>
    typename std::enable_if<!labelset_t_of<Aut>::has_one(),
                            detail::blind_automaton<0, Aut>>::type
    get_insplit(Aut& aut)
    {
      return make_blind_automaton<0>(aut);
    }

  }

  /*--------------------------------.
  | compose(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) composition.
  template <typename Lhs, typename Rhs>
  auto
  compose(Lhs& lhs, Rhs& rhs)
    -> typename detail::composer<detail::blind_automaton<1, Lhs>,
                                 detail::blind_automaton<0, Rhs>>::automaton_t
  {
    auto l = sort(detail::make_blind_automaton<1>(lhs));
    auto r = sort(detail::get_insplit(rhs));
    detail::composer<decltype(l), decltype(r)>compose(l, r);
    auto res = compose.compose();
    if (getenv("VCSN_ORIGINS"))
      compose.print(std::cout, compose.origins());
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      compose(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(compose(l, r));
      }

      REGISTER_DECLARE(compose,
                       (const automaton&, const automaton&) -> automaton);
    }
  }

}


#endif /* !VCSN_ALGOS_COMPOSE_HH */
