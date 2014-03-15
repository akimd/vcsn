#ifndef VCSN_ALGOS_SORT_HH
# define VCSN_ALGOS_SORT_HH

# include <algorithm>
# include <map>
# include <queue>
# include <unordered_map>
# include <vector>

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*-------.
  | sort.  |
  `-------*/
  namespace detail
  {

    template <typename Aut>
    class sorter
    {
      using automaton_t = Aut;
      using context_t = typename automaton_t::context_t;
      using weight_t = typename automaton_t::weight_t;
      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;

      using labelset_t = typename context_t::labelset_t;
      using weightset_t = typename context_t::weightset_t;
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
        map_[a_.pre()] = res_.pre();
        map_[a_.post()] = res_.post();
        worklist_.push({a_.pre(), res_.pre()});
      }

      void visit_successors_of(state_t s, res_state_t res_s)
      {
        std::vector<transition_t> tt;
        // Here a_.out(s) would just as well as a_.all_out(s) but it
        // would be slower; later we have to test one condition per
        // transition anyway, which is just the additional work
        // performed by out.
        for (auto t: a_.all_out(s))
          tt.emplace_back(t);

        auto closure =
          [&](const transition_t t1,
              const transition_t t2) -> bool
          {
            return transition_precedes(t1, t2);
          };
        std::sort(tt.begin(), tt.end(), closure);

        for (auto t: tt)
          {
            state_t dst = a_.dst_of(t);
            res_state_t res_dst;
            if (map_.find(dst) == map_.end())
              res_dst = treat_state(dst);
            else
              res_dst = map_.at(dst);
            res_.new_transition(res_s,
                                res_dst,
                                a_.label_of(t),
                                a_.weight_of(t));
          }
      }

      /// Also return the res_ state.  This lets the caller avoid hash accesses.
      res_state_t treat_state(state_t s)
      {
        state_t res = res_.new_state();
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
        std::vector<state_t> ss;
        for (auto s: a_.all_states()) // Like above, a_.states_() would work.
          if (map_.find(s) == map_.end())
            ss.emplace_back(s);

        // Just sort by input state number, without looking at
        // incoming or outgoing transitions.  This is crude, but
        // acceptable in most situations.
        std::sort(ss.begin(), ss.end());

        for (auto s: ss)
          treat_state(s);
      }

      bool transition_precedes(const transition_t t1,
                               const transition_t t2) const
      {
        // We intentionally ignore source states: they should always
        // be identical when we call this.
        require(a_.src_of(t1) == a_.src_of(t2),
                __func__, ": not the same source state");
        if (ls_.less_than(a_.label_of(t1), a_.label_of(t2)))
          return true;
        else if (ls_.less_than(a_.label_of(t2), a_.label_of(t1)))
          return false;
        else if (ws_.less_than(a_.weight_of(t1), a_.weight_of(t2)))
          return true;
        else if (ws_.less_than(a_.weight_of(t2), a_.weight_of(t1)))
          return false;
        else if (a_.dst_of(t1) < a_.dst_of(t2))
          return true;
        else if (a_.dst_of(t2) < a_.dst_of(t1))
          return false;
        else
          return false; // The compiler optimizer will "factor" for us.
      }

    public:
      sorter(const automaton_t& a)
        : a_(a)
        , ls_(* a.labelset())
        , ws_(* a.weightset())
        , res_(a_.context())
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

      /*--------------.
      | Bridge: sort  |
      `--------------*/

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
