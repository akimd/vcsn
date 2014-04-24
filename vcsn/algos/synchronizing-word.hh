#ifndef VCSN_ALGOS_SYNCHRONIZING_WORD_HH
# define VCSN_ALGOS_SYNCHRONIZING_WORD_HH

# include <algorithm>
# include <iostream>
# include <map>
# include <queue>
# include <set>
# include <unordered_set>
# include <utility>
# include <vector>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/distance.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/label.hh>
# include <vcsn/misc/pair.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*--------------------------------------.
  | is_synchronized_by(automaton, word).  |
  `--------------------------------------*/

  template <typename Aut>
  bool
  is_synchronized_by(const Aut& aut,
                     const typename Aut::labelset_t::word_t& w)
  {
    using context_t = typename Aut::context_t;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;

    std::unordered_set<state_t> todo;

    for (auto s : aut.states())
      todo.insert(s);

    for (auto l : aut.labelset()->letters_of(w))
      {
        std::unordered_set<state_t> new_todo;
        for (auto s : todo)
          {
            auto ntf = aut.out(s, l);
            auto size = ntf.size();
            require(0 < size, "automaton must be complete");
            require(size < 2, "automaton must be deterministic");
            new_todo.insert(aut.dst_of(*ntf.begin()));
          }
        todo = std::move(new_todo);
      }

    return todo.size() == 1;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename LabelSet>
      bool
      is_synchronized_by(const automaton& aut, const label& word)
      {
        const auto& a = aut->as<Aut>();
        const auto& w = word->as<LabelSet>();
        return vcsn::is_synchronized_by(a, w.label());
      }

      REGISTER_DECLARE(is_synchronized_by,
                       (const automaton&, const label&) -> bool);
    }
  }

  namespace detail
  {
    template <typename Aut>
    class pairer
    {
    public:
      using context_t = typename Aut::context_t;
      using automaton_t =  mutable_automaton<context_t>;
      using state_t = typename automaton_t::state_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename weightset_t::value_t;

    private:
      using pair_t = std::pair<state_t, state_t>;

    public:
      pairer(const Aut& aut)
        : aut_(aut)
      {}

      Aut pair()
      {
        auto ctx = aut_.context();
        auto ws = ctx.weightset();

        automaton_t res(ctx);
        q0_ = res.new_state(); // q0 special state

        for (auto l : *aut_.labelset())
          res.add_transition(q0_, q0_, l, ws->one());

        for (auto s1: aut_.states())
          for (auto s2: aut_.states())
            if (s1 != s2)
              {
                auto np = make_ordered_pair(s1, s2);
                if (pair_states_.find(np) == pair_states_.end())
                  pair_states_[np] = res.new_state();
              }

        for (auto ps : pair_states_)
          {
            auto pstates = ps.first; // pair of states
            auto cstate = ps.second; // current state

            for (auto t1: aut_.out(pstates.first))
              {
                auto label = aut_.label_of(t1);
                auto dst1 = aut_.dst_of(t1);
                for (auto t2: aut_.out(pstates.second, label))
                  {
                    auto dst2 = aut_.dst_of(t2);
                    weight_t nw = ws->add(aut_.weight_of(t1),
                                          aut_.weight_of(t2));
                    if (dst1 != dst2)
                      {
                        auto np = make_ordered_pair(dst1, dst2);
                        res.add_transition(cstate,
                                           pair_states_.find(np)->second,
                                           label, nw);
                      }
                    else
                        res.add_transition(cstate, q0_, label, nw);
                  }
              }
          }

        called_ = true;
        return res;
      }

      const std::unordered_map<pair_t, state_t>& get_map_pair()
      {
        require(called_,
                "trying to call get_map_pair() before calling pair()");
        return pair_states_;
      }

      state_t get_q0()
      {
        require(called_, "trying to call get_q0() before calling pair()");
        return q0_;
      }

    private:
      const Aut& aut_;
      std::unordered_map<pair_t, state_t> pair_states_;
      state_t q0_;
      bool called_ = false;
    };
  }

  /*------------------.
  | pair(automaton).  |
  `------------------*/

  template <typename Aut>
  Aut pair(const Aut& aut)
  {
    detail::pairer<Aut> sw(aut);
    return sw.pair();
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton
      pair(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(pair(a));
      }

      REGISTER_DECLARE(pair, (const automaton&) -> automaton);
    }
  }

  template <typename Aut>
  typename Aut::labelset_t::word_t
  synchronizing_word(const Aut& aut)
  {
    using automaton_t = Aut;
    using word_t = typename automaton_t::labelset_t::word_t;
    using state_t = typename automaton_t::state_t;
    using transition_t = typename automaton_t::transition_t;

    word_t res;
    std::unordered_set<state_t> todo;

    detail::pairer<Aut> pobj(aut);
    Aut pa = pobj.pair();
    state_t q0 = pobj.get_q0();

    std::unordered_map<state_t, std::pair<state_t, transition_t>> paths =
        paths_ibfs(pa, q0);

    if (paths.size() < pa.states().size() - 1)
        raise("automaton is not synchronizing.");

    for (auto s : pa.states())
      todo.insert(s);

    while (1 < todo.size() || todo.find(q0) == todo.end())
      {
        int min = -1;
        std::vector<transition_t> path;
        for (auto s : todo)
          if (s != q0)
            {
              std::vector<transition_t> cpath;
              state_t bt_curr = s;
              while (bt_curr != q0)
                {
                  transition_t t;
                  std::tie(bt_curr, t) = paths.find(bt_curr)->second;
                  cpath.push_back(t);
                }

              if (min == -1 || min > static_cast<long>(cpath.size()))
                {
                  min = cpath.size();
                  path = cpath;
                }
            }
        for (auto t : path)
          {
            auto l = pa.label_of(t);
            std::unordered_set<state_t> new_todo;
            for (auto s : todo)
              {
                auto ntf = pa.out(s, l);
                if (ntf.empty())
                  raise("automaton must be complete");
                if (1 < ntf.size())
                  raise("automaton must be deterministic");
                new_todo.insert(pa.dst_of(*ntf.begin()));
              }
            todo = std::move(new_todo);
            res = aut.labelset()->concat(res, l);
          }
      }

    return res;
  }

  /*-------------------------------.
  | synchronizing_word(automaton). |
  `-------------------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      label
      synchronizing_word(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_label(make_wordset(*a.labelset()), vcsn::synchronizing_word(a));
      }

      REGISTER_DECLARE(synchronizing_word, (const automaton&) -> label);
    }
  }
}

#endif // !VCSN_ALGOS_SYNCHRONIZING_WORD_HH
