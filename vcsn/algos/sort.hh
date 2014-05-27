#ifndef VCSN_ALGOS_SORT_HH
# define VCSN_ALGOS_SORT_HH

# include <map>
# include <queue>
# include <vector>

# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/algorithm.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/unordered_map.hh>

namespace vcsn
{


  /*------------------.
  | is_label_sorted.  |
  `------------------*/

  /// Whether for each state, the outgoing transitions are sorted by
  /// increasing label.
  template <typename Aut>
  inline
  bool
  is_out_sorted(const Aut& a)
  {
    using transition_t = transition_t_of<Aut>;
    for (state_t_of<Aut> s: a->states())
      if (!detail::is_sorted(a->out(s),
                             [&a] (transition_t l, transition_t r)
                             {
                               return a->labelset()->less_than(a->label_of(l),
                                                              a->label_of(r));
                             }))
        return false;
    return true;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Aut>
      bool
      is_out_sorted(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_out_sorted(a);
      }

      REGISTER_DECLARE(is_out_sorted,
                       (const automaton&) -> bool);
    }
  }


  /*-------.
  | sort.  |
  `-------*/
  namespace detail
  {

    template <typename Aut>
    class sorter
    {
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<context_t>;
      const automaton_t& a_;
      const labelset_t& ls_;
      const weightset_t& ws_;

      automaton_t res_;
      using res_state_t = state_t;

      using pair_t = std::pair<state_t, res_state_t>;
      std::queue<pair_t> worklist_;

      /// The map we're computing.
      std::unordered_map<state_t, res_state_t> map_;

      void initialize()
      {
        worklist_ = std::move(std::queue<pair_t>()); // There's no clear method.
        map_.clear();
        map_[a_->pre()] = res_->pre();
        map_[a_->post()] = res_->post();
        worklist_.push({a_->pre(), res_->pre()});
      }

      void visit_successors_of(state_t s, res_state_t res_s)
      {
        std::vector<transition_t> tt;
        // Here a_->out(s) would just as well as a_->all_out(s) but it
        // would be slower; later we have to test one condition per
        // transition anyway, which is just the additional work
        // performed by out.
        for (auto t: a_->all_out(s))
          tt.emplace_back(t);

        std::sort(tt.begin(), tt.end(),
                  [&](const transition_t t1,
                      const transition_t t2) -> bool
                  {
                    return transition_less_than(t1, t2);
                  });

        for (auto t: tt)
          {
            state_t dst = a_->dst_of(t);
            res_state_t res_dst;
            if (map_.find(dst) == map_.end())
              res_dst = treat_state(dst);
            else
              res_dst = map_.at(dst);
            res_->new_transition_copy(a_, res_s,
                                      res_dst,
                                      t,
                                      a_->weight_of(t));
          }
      }

      /// Also return the res_ state.  This lets the caller avoid hash accesses.
      res_state_t treat_state(state_t s)
      {
        state_t res = res_->new_state();
        map_[s] = res;
        worklist_.push({s, res});
        return res;
      }

      void visit_and_update_res()
      {
        while (! worklist_.empty())
          {
            pair_t p = worklist_.front(); worklist_.pop();
            state_t s = p.first; res_state_t res_s = p.second;
            visit_successors_of(s, res_s);
          } // while
      }

      void push_inaccessible_states()
      {
        // States are processed in order.
        for (auto s: a_->all_states()) // Like above, a_->states_() would work.
          if (!has(map_, s))
            treat_state(s);
      }

      bool transition_less_than(const transition_t t1,
                                const transition_t t2) const
        ATTRIBUTE_PURE
      {
        // We intentionally ignore source states: they should always
        // be identical when we call this.
        assert(a_->src_of(t1) == a_->src_of(t2));
        if (ls_.less_than(a_->label_of(t1), a_->label_of(t2)))
          return true;
        else if (ls_.less_than(a_->label_of(t2), a_->label_of(t1)))
          return false;
        else if (ws_.less_than(a_->weight_of(t1), a_->weight_of(t2)))
          return true;
        else if (ws_.less_than(a_->weight_of(t2), a_->weight_of(t1)))
          return false;
        else if (a_->dst_of(t1) < a_->dst_of(t2))
          return true;
        else if (a_->dst_of(t2) < a_->dst_of(t1))
          return false;
        else
          return false; // The compiler optimizer will "factor" for us.
      }

    public:
      sorter(const automaton_t& a)
        : a_(a)
        , ls_(*a_->labelset())
        , ws_(*a_->weightset())
        , res_(std::make_shared<typename automaton_t::element_type>(a_->context()))
      {}

      automaton_t operator()()
      {
        initialize();
        visit_and_update_res();
        push_inaccessible_states();
        visit_and_update_res();
        return std::move(res_);
      }

      /// A map from renamed state to original state.
      using origins_t = std::map<res_state_t, state_t>;
      origins_t
      origins()
      {
        origins_t res;
        for (const auto& pair: map_)
          if (pair.second != pair.first)
            res[pair.second] = pair.first;
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins." << std::endl
          << "    node [shape = box, style = rounded]" << std::endl;
        for (auto p : orig)
          if (2 <= p.first)
            o << "    " << p.first - 2
              << " [label = \"" << p.second - 2 << "\"]"
              << std::endl;
        o << "*/" << std::endl;
        return o;
      }
    }; // class
  } // namespace

  template <typename Aut>
  inline
  Aut
  sort(const Aut& a)
  {
    detail::sorter<Aut> sorter(a);
    auto res = sorter();
    // FIXME: Not terribly elegant.  But currently there's no mean to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      {
        auto o = sorter.origins();
        if (!o.empty())
          sorter.print(std::cout, o);
      }
    return res;
  }

  namespace dyn
  {
    namespace detail
    {

      /// Bridge.
      template <typename Aut>
      automaton
      sort(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(sort(a));
      }

      REGISTER_DECLARE(sort,
                       (const automaton&) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_SORT_HH
