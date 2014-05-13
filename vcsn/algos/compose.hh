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
            typename Aut::labelset_t::valuesets_t>::type;

      using type = tupleset<elem<I>...>;
    };

    /// Read-write on an automaton, that hides all bands but one.
    template <std::size_t Band, typename Aut>
    class blind_automaton : public automaton_decorator<Aut>
    {
    public:
      /// The type of automaton to wrap.
      using automaton_t = Aut;
      using super = automaton_decorator<Aut>;

      static_assert(Aut::context_t::is_lat, "requires labels_are_tuples");
      static_assert(Aut::context_t::labelset_t::size() > Band, "band outside of the tuple");

      /// The type of the automata to produce from this kind o
      /// automata.  For instance, insplitting on a
      /// blind_automaton<const mutable_automaton<Ctx>> should
      /// yield a blind_automaton<mutable_automaton<Ctx>>, without
      /// the "inner" const.
      using self_nocv_t
        = blind_automaton<Band, typename automaton_t::self_nocv_t>;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;
      // Exposed label
      using label_t =
        typename std::tuple_element<Band, typename automaton_t::label_t>::type;
      // Underlying automaton label
      using hidden_label_t = typename automaton_t::label_t;
      using weight_t = typename automaton_t::weight_t;
      using hidden_indices_t = concat_sequence<
                               typename make_index_range<0, Band>::type,
                               typename make_index_range<Band + 1,
                                 std::tuple_size<hidden_label_t>::value
                                   - Band - 1>::type>;

      using labelset_t = typename std::tuple_element<Band,
            typename automaton_t::labelset_t::valuesets_t>::type;
      using hidden_labelset_t = typename automaton_t::labelset_t;

      // All bands except the exposed one
      using res_labelset_t = typename hidden_label_type<Aut, hidden_indices_t>::type;
      using res_label_t = typename res_labelset_t::value_t;
      using weightset_t = typename automaton_t::weightset_t;

      using labelset_ptr = std::shared_ptr<const labelset_t>;
      using context_t = ::vcsn::context<res_labelset_t, weightset_t>;

      using weightset_ptr = typename automaton_t::weightset_ptr;

    public:
      using super::automaton_decorator;

      blind_automaton(const typename automaton_t::context_t& ctx)
        : blind_automaton(new automaton_t{ctx})
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

      /*------------------------------.
      | non-const forwarded methods.  |
      `------------------------------*/

# define DEFINE(Name)                                               \
      template <typename... Args>                                   \
      auto Name(Args&&... args)                                     \
        -> decltype(this->aut_->Name(std::forward<Args>(args)...))  \
      {                                                             \
        return this->aut_->Name(std::forward<Args>(args)...);       \
      }

      DEFINE(new_state);

# undef DEFINE

      template <typename A>
      transition_t new_transition_copy(const A& aut, state_t src,
                                       state_t dst, transition_t t, weight_t k)
      {
        return this->aut_->new_transition_copy(*const_cast<A*>(&aut)->
                                                 original_automaton(),
                                               src, dst, t, k);
      }

      template <typename A>
      weight_t add_transition_copy(const A& aut, state_t src,
                                   state_t dst, transition_t t, weight_t k)
      {
        return this->aut_->add_transition_copy(*const_cast<A*>(&aut)
                                                 ->original_automaton(),
                                               src, dst, t, k);
      }

      /*------------------------------.
      | constexpr forwarded methods.  |
      `------------------------------*/

# define DEFINE(Name)                           \
      static constexpr                          \
      auto                                      \
      Name()                                    \
        -> decltype(automaton_t::Name())        \
      {                                         \
        return automaton_t::Name();             \
      }

      DEFINE(post);
      DEFINE(pre);

#undef DEFINE

      /*--------------------------.
      | forwarded const methods.  |
      `--------------------------*/

      const typename automaton_t::context_t& context() const
      {
        return this->aut_->context();
      }

# define DEFINE(Name)                                                 \
      template <typename... Args>                                     \
      auto                                                            \
      Name(Args&&... args) const                                      \
        -> decltype(this->aut_->Name(std::forward<Args>(args)...))    \
      {                                                               \
        return this->aut_->Name(std::forward<Args>(args)...);         \
      }

      DEFINE(is_initial);
      DEFINE(is_final);
      DEFINE(all_in);
      DEFINE(all_out);
      DEFINE(in);
      DEFINE(out);
      DEFINE(outin);
      DEFINE(src_of);
      DEFINE(dst_of);
      DEFINE(initial_transitions);
      DEFINE(final_transitions);

      DEFINE(get_initial_weight);

      DEFINE(get_final_weight);

      DEFINE(weight_of);
# undef DEFINE
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
      static_assert(Lhs::context_t::is_lat,
                    "requires labels_are_tuples");
      static_assert(Rhs::context_t::is_lat,
                    "requires labels_are_tuples");

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

    public:
      using clhs_t = Lhs;
      using crhs_t = Rhs;
      using hidden_l_label_t = typename clhs_t::res_label_t;
      using hidden_r_label_t = typename crhs_t::res_label_t;
      using hidden_l_labelset_t = typename clhs_t::res_labelset_t;
      using hidden_r_labelset_t = typename crhs_t::res_labelset_t;
      using middle_labelset_t = typename clhs_t::labelset_t;
      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltration.
      using weightset_t = join_t<typename Lhs::context_t::weightset_t,
                                 typename Rhs::context_t::weightset_t>;
      using labelset_t = typename concat_tupleset<hidden_l_labelset_t,
                                                  hidden_r_labelset_t>::type;

      using res_label_t = typename labelset_t::value_t;
      using context_t = ::vcsn::context<labelset_t, weightset_t>;

      /// The type of the resulting automaton.
      using automaton_t = mutable_automaton<context_t>;

      /// Result state type.
      using state_t = typename automaton_t::state_t;
      /// Tuple of states of input automata.
      using pair_t = std::pair<typename Lhs::state_t, typename Rhs::state_t>;
      /// A map from result state to tuple of original states.
      using origins_t = std::map<state_t, pair_t>;

      composer(const Lhs& lhs, const Rhs& rhs)
        : lhs_(lhs)
        , rhs_(rhs)
        , res_(context_t{make_labelset_(lhs_.res_labelset(), rhs_.res_labelset()),
                       join(*lhs.weightset(), *rhs.weightset())})
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
        return labelset_t{std::get<I1>(ll.sets())..., std::get<I2>(rl.sets())...};
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
        const auto& ws = *res_.context().weightset();

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
          if (p.first != automaton_t::pre() && p.first != automaton_t::post())
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
        return pair_t{lhs_.pre(), rhs_.pre()};
      }

      /// The post of the input automata.
      pair_t post_() const
      {
        return pair_t{lhs_.post(), rhs_.post()};
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
        pmap_[pre_()] = res_.pre();
        pmap_[post_()] = res_.post();
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
      state_t state(typename Lhs::state_t ls, typename Rhs::state_t rs)
      {
        pair_t state{ls, rs};
        auto lb = pmap_.lower_bound(state);
        if (lb == pmap_.end() || pmap_.key_comp()(state, lb->first))
          {
            lb = pmap_.emplace_hint(lb, state, res_.new_state());
            todo_.emplace_back(state);
          }
        return lb->second;
      }

      res_label_t join_label(hidden_l_label_t ll, hidden_r_label_t rl)
      {
        return std::tuple_cat(ll, rl);
      }

      template<typename Aut>
      typename std::enable_if<Aut::labelset_t::has_one(),
                              typename Aut::res_label_t>::type
      get_hidden_one(const Aut& aut)
      {
        return aut.hidden_one();
      }

      template<typename Aut>
      typename std::enable_if<!Aut::labelset_t::has_one(),
                              typename Aut::res_label_t>::type
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
        auto ls = lhs_.all_out(psrc.first);
        auto rs = rhs_.all_out(psrc.second);
        auto li = ls.begin();
        auto ri = rs.begin();

        for (/* Nothing. */; li != ls.end() && is_one(lhs_, *li); ++li)
          if (!has_only_ones_in(rhs_, psrc.second))
            res_.new_transition(src, state(lhs_.dst_of(*li), psrc.second),
                                join_label(lhs_.hidden_label_of(*li),
                                           get_hidden_one(rhs_)),
                                ws.mul(lhs_.weight_of(*li),
                                       rhs_.context().weightset()->one()));

        for (/* Nothing. */; ri != rs.end() && is_one(rhs_, *ri); ++ri)
          res_.new_transition(src, state(psrc.first, rhs_.dst_of(*ri)),
                              join_label(get_hidden_one(lhs_),
                                         rhs_.hidden_label_of(*ri)),
                              ws.mul(lhs_.context().weightset()->one(),
                                     rhs_.weight_of(*ri)));


        for (/* Nothing. */;
             li != ls.end() && ri != rs.end();
             ++ li)
        {
          auto lt = *li;
          typename clhs_t::label_t label = lhs_.label_of(lt);
          // Skip right-hand transitions with labels we don't have
          // on the left hand.
          while (middle_labelset_t::less_than(rhs_.label_of(*ri), label))
            if (++ ri == rs.end())
              return;

          // If the smallest label on the right-hand side is bigger
          // than the left-hand one, we have no hope of ever adding
          // transitions with this label.
          if (middle_labelset_t::less_than(label, rhs_.label_of(*ri)))
            continue;

          assert(middle_labelset_t::equals(label, rhs_.label_of(*ri)));
          auto rstart = ri;
          while (middle_labelset_t::equals(rhs_.label_of(*ri), label))
          {
            // These are always new transitions: first because the
            // source state is visited for the first time, and
            // second because the couple (left destination, label)
            // is unique, and so is (right destination, label).
            res_.new_transition(src, state(lhs_.dst_of(lt), rhs_.dst_of(*ri)),
                                join_label(lhs_.hidden_label_of(*li),
                                           rhs_.hidden_label_of(*ri)),
                                ws.mul(lhs_.weight_of(lt),
                                       rhs_.weight_of(*ri)));

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
      typename std::enable_if<A::context_t::labelset_t::has_one(),
                              bool>::type
      is_one(const A& aut, typename A::transition_t tr) const
      {
        return aut.labelset()->is_one(aut.label_of(tr));
      }

      template <typename A>
      constexpr typename std::enable_if<!A::context_t::labelset_t::has_one(),
                              bool>::type
      is_one(const A&, typename A::transition_t)
      const
      {
        return false;
      }

      template <typename Aut>
      constexpr typename std::enable_if<!Aut::context_t::labelset_t::has_one(),
                  bool>::type
      has_only_ones_in(const Aut&,
                       typename Aut::state_t) const
      {
        return false;
      }

      template <typename Aut>
      typename std::enable_if<Aut::context_t::labelset_t::has_one(),
                 bool>::type
      has_only_ones_in(const Aut& rhs, typename Aut::state_t rst) const
      {
        auto rin = rhs.all_in(rst);
        auto rtr = rin.begin();
        return rtr != rin.end() && is_one(rhs, *rtr) && !rhs.is_initial(rst);
      }


      /// The computed product.
      automaton_t res_;
    };

  }

  /*--------------------------------.
  | compose(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) composition.
  template <typename Lhs, typename Rhs>
  auto
  compose(const Lhs& lhs, const Rhs& rhs)
    -> typename detail::composer<detail::blind_automaton<1, const Lhs>,
                                 typename detail::blind_automaton<0, const Rhs>
                                   ::self_nocv_t>::automaton_t
  {
    // We need a local variable for correct scope
    typename detail::blind_automaton<1, const Lhs>::self_nocv_t l
      = sort(detail::blind_automaton<1, const Lhs>{lhs});
    typename detail::blind_automaton<0, const Rhs>::self_nocv_t r
      = sort(insplit(detail::blind_automaton<0, const Rhs>{rhs})); // Same here
    detail::composer<typename detail::blind_automaton<1, const Lhs>::self_nocv_t,
                     typename detail::blind_automaton<0, const Rhs>::self_nocv_t>
                       compose(l, r);
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
