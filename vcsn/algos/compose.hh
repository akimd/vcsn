#ifndef VCSN_ALGOS_COMPOSE_HH
# define VCSN_ALGOS_COMPOSE_HH

# include <deque>
# include <iostream>
# include <map>

# include <vcsn/algos/blind.hh>
# include <vcsn/algos/insplit.hh>
# include <vcsn/algos/sort.hh>
# include <vcsn/core/tuple-automaton.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/labelset/tupleset.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/tuple.hh> // make_index_sequence

namespace vcsn
{
  namespace detail
  {
    /*---------------------------------.
    | composer<automaton, automaton>.  |
    `---------------------------------*/

    /// Build the (accessible part of the) composition.
    template <typename Lhs, typename Rhs>
    class composer
    {
      static_assert(Lhs::element_type::full_context_t::is_lat,
                    "requires labels_are_tuples");
      static_assert(Rhs::element_type::full_context_t::is_lat,
                    "requires labels_are_tuples");

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

    public:
      using clhs_t = Lhs;
      using crhs_t = Rhs;
      // clhs_t and crhs_t are permutation automata, yet we need to
      // read the res_label_t from their wrapped automaton type.
      using hidden_l_labelset_t = typename clhs_t::element_type::res_labelset_t;
      using hidden_r_labelset_t = typename crhs_t::element_type::res_labelset_t;
      using hidden_l_label_t = typename hidden_l_labelset_t::value_t;
      using hidden_r_label_t = typename hidden_r_labelset_t::value_t;

      static_assert(std::is_same<labelset_t_of<clhs_t>,
                                 labelset_t_of<crhs_t>>::value,
                    "common tape must be of same type");

      using middle_labelset_t = labelset_t_of<clhs_t>;
      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltration.
      using labelset_t = typename concat_tupleset<hidden_l_labelset_t,
                                                  hidden_r_labelset_t>::type;
      using weightset_t = join_t<weightset_t_of<context_t_of<Lhs>>,
                                 weightset_t_of<context_t_of<Rhs>>>;

      using res_label_t = typename labelset_t::value_t;
      using context_t = ::vcsn::context<labelset_t, weightset_t>;

      /// The type of the resulting automaton.
      using automaton_t = tuple_automaton<mutable_automaton<context_t>,
                                          Lhs, Rhs>;

      /// Result state type.
      using state_t = state_t_of<automaton_t>;
      /// Tuple of states of input automata.
      using pair_t = typename automaton_t::element_type::pair_t;

      composer(const Lhs& lhs, const Rhs& rhs)
        : res_(make_shared_ptr<automaton_t>(make_mutable_automaton(make_context_(lhs, rhs)),
                                            lhs, rhs))
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

      static context_t
      make_context_(const Lhs& lhs, const Rhs& rhs)
      {
        return {make_labelset_(lhs->res_labelset(), rhs->res_labelset()),
                join(*lhs->weightset(), *rhs->weightset())};
      }

      /// The (accessible part of the) product of \a lhs_ and \a rhs_.
      automaton_t compose()
      {
        initialize_compose();
        const auto& ws = *res_->context().weightset();

        while (!res_->todo_.empty())
          {
            pair_t psrc = res_->todo_.front();
            res_->todo_.pop_front();
            state_t src = res_->pmap_[psrc];

            add_compose_transitions(ws, src, psrc);
          }
        return std::move(res_);
      }

    private:
      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      /// Fill the worklist with the initial source-state pairs, as
      /// needed for the product algorithm.
      void initialize_compose()
      {
        res_->todo_.emplace_back(res_->pre_());
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
        // the two successor lists in lockstep. Thus if there is a
        // sponteneous transition, it is at the beginning.
        auto& lhs = std::get<0>(res_->auts_);
        auto& rhs = std::get<1>(res_->auts_);
        auto ls = lhs->all_out(std::get<0>(psrc));
        auto rs = rhs->all_out(std::get<1>(psrc));
        auto li = ls.begin();
        auto ri = rs.begin();

        for (/* Nothing. */; li != ls.end() && is_one(lhs, *li); ++li)
          if (!has_only_ones_in(rhs, std::get<1>(psrc)))
            res_->new_transition(src,
                                 res_->state(lhs->dst_of(*li), std::get<1>(psrc)),
                                 join_label(lhs->hidden_label_of(*li),
                                            get_hidden_one(rhs)),
                                 ws.conv(*lhs->weightset(), lhs->weight_of(*li)));

        for (/* Nothing. */; ri != rs.end() && is_one(rhs, *ri); ++ri)
          res_->new_transition(src,
                               res_->state(std::get<0>(psrc), rhs->dst_of(*ri)),
                               join_label(get_hidden_one(lhs),
                                          rhs->hidden_label_of(*ri)),
                               ws.conv(*rhs->weightset(), rhs->weight_of(*ri)));


        for (/* Nothing. */;
             li != ls.end() && ri != rs.end();
             ++ li)
        {
          auto lt = *li;
          label_t_of<clhs_t> label = lhs->label_of(lt);
          // Skip right-hand transitions with labels we don't have
          // on the left hand.
          while (middle_labelset_t::less_than(rhs->label_of(*ri), label))
            if (++ ri == rs.end())
              return;

          // If the smallest label on the right-hand side is bigger
          // than the left-hand one, we have no hope of ever adding
          // transitions with this label.
          if (middle_labelset_t::less_than(label, rhs->label_of(*ri)))
            continue;

          assert(middle_labelset_t::equals(label, rhs->label_of(*ri)));
          auto rstart = ri;
          while (middle_labelset_t::equals(rhs->label_of(*ri), label))
          {
            // These are always new transitions: first because the
            // source state is visited for the first time, and
            // second because the couple (left destination, label)
            // is unique, and so is (right destination, label).
            res_->new_transition(src, res_->state(lhs->dst_of(lt), rhs->dst_of(*ri)),
                                 join_label(lhs->hidden_label_of(*li),
                                            rhs->hidden_label_of(*ri)),
                                 ws.mul(ws.conv(*lhs->weightset(),
                                                lhs->weight_of(lt)),
                                        ws.conv(*rhs->weightset(),
                                                rhs->weight_of(*ri))));
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
  }

  /*--------------------------------.
  | compose(automaton, automaton).  |
  `--------------------------------*/

  /// Build the (accessible part of the) composition.
  template <typename Lhs, typename Rhs>
  auto
  compose(Lhs& lhs, Rhs& rhs)
    -> typename detail::composer<blind_automaton<1, Lhs>,
                                 blind_automaton<0, Rhs>>::automaton_t
  {
    auto l = sort(blind<1>(lhs))->strip();
    auto r = sort(insplit(blind<0>(rhs)))->strip();
    detail::composer<decltype(l), decltype(r)> compose(l, r);
    return compose.compose();
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      compose(automaton& lhs, automaton& rhs)
      {
        auto& l = lhs->as<Lhs>();
        auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::compose(l, r));
      }

      REGISTER_DECLARE(compose,
                       (automaton&, automaton&) -> automaton);
    }
  }

}


#endif // !VCSN_ALGOS_COMPOSE_HH
