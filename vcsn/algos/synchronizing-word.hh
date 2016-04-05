#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <vcsn/algos/distance.hh>
#include <vcsn/algos/pair.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/labelset/labelset.hh> // make_wordset
#include <vcsn/misc/algorithm.hh> // erase_if
#include <vcsn/misc/map.hh>
#include <vcsn/misc/pair.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*--------------------------------------.
  | is_synchronized_by(automaton, word).  |
  `--------------------------------------*/

  /// Whether \a w synchronizes automaton \a aut.
  template <Automaton Aut>
  bool
  is_synchronized_by(const Aut& aut, const word_t_of<Aut>& w)
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;

    std::unordered_set<state_t> todo;

    for (auto s : aut->states())
      todo.insert(s);

    for (auto l : aut->labelset()->letters_of(w))
      {
        std::unordered_set<state_t> new_todo;
        for (auto s : todo)
          {
            auto ntf = out(aut, s, l);
            auto size = ntf.size();
            require(0 < size,
                    "is_synchronized_by: automaton must be complete");
            require(size < 2,
                    "is_synchronized_by: automaton must be deterministic");
            new_todo.insert(aut->dst_of(*ntf.begin()));
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
      template <Automaton Aut, typename LabelSet>
      bool
      is_synchronized_by(const automaton& aut, const label& word)
      {
        const auto& a = aut->as<Aut>();
        const auto& w = word->as<LabelSet>();
        return vcsn::is_synchronized_by(a, w.value());
      }
    }
  }


  /*---------------------.
  | word_synchronizer.   |
  `---------------------*/

  namespace detail
  {
    template <Automaton Aut>
    class word_synchronizer
    {
    public:
      using automaton_t = Aut;
      using word_t = word_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

    private:
      using state_t = state_t_of<automaton_t>;

      using pair_automaton_t = pair_automaton<automaton_t>;
      using state_name_t = std::pair<state_t, state_t>;

      using dist_transition_t
        = std::pair<unsigned, transition_t_of<pair_automaton_t>>;
      using paths_t
        = std::unordered_map<state_t_of<pair_automaton_t>, dist_transition_t>;
      using path_t = typename paths_t::value_type;

      /// Input automaton.
      automaton_t aut_;
      /// Corresponding pair automaton.
      pair_automaton_t pair_;
      paths_t paths_;
      std::unordered_set<state_t> todo_;
      word_t res_;

    public:
      word_synchronizer(const automaton_t& aut)
        : aut_(aut)
      {}

    private:
      void init_pair(bool keep_initials = false)
      {
        pair_ = pair(aut_, keep_initials);
        paths_ = paths_ibfs(pair_, pair_->singletons());

        if (keep_initials)
          detail::erase_if(paths_,
                           [this](const path_t& p)
                           {
                             return pair_->is_singleton(p.first);
                           });
      }

      void init_synchro(bool keep_initials = false)
      {
        init_pair(keep_initials);
        require(pair_->states().size()
                == paths_.size() + pair_->singletons().size(),
                "automaton is not synchronizing");

        for (auto s : pair_->states())
          if (!pair_->is_singleton(s))
            todo_.insert(s);
      }

      std::vector<transition_t> recompose_path(state_t from) const
      {
        std::vector<transition_t> res;
        state_t bt_curr = from;
        while (!pair_->is_singleton(bt_curr))
          {
            transition_t t = paths_.find(bt_curr)->second.second;
            res.push_back(t);
            bt_curr = pair_->dst_of(t);
          }
        return res;
      }

      int dist(state_t s) const
      {
        return pair_->is_singleton(s) ? 0 : paths_.find(s)->second.first;
      }

      state_t dest_state(state_t s, const label_t& l) const
      {
        auto ntf = out(pair_, s, l);
        auto size = ntf.size();
        require(0 < size, "automaton must be complete");
        require(size < 2, "automaton must be deterministic");
        return pair_->dst_of(*ntf.begin());
      }

      void apply_label(const label_t& label)
      {
        res_ = aut_->labelset()->mul(res_, label);
        std::unordered_set<state_t> new_todo;
        for (auto s : todo_)
          {
            auto new_state = dest_state(s, label);
            if (!pair_->is_singleton(new_state))
              new_todo.insert(new_state);
          }
        todo_ = std::move(new_todo);
      }

      /// "Apply" a word to the set of active states (for each state,
      /// for each label, perform s = d(s))
      void apply_path(const std::vector<transition_t>& path)
      {
        for (auto t : path)
          apply_label(pair_->label_of(t));
      }

    public:

      // We just perform an inverse BFS from q0 and put all the accessible
      // states in 'paths'. If the size of paths is the same than the number
      // of states of pa (minus q0), then for each pair of states (p, q),
      // there is a word w such that d(p, w) = d(q, w), thus the automaton is
      // synchronizing.
      bool is_synchronizing()
      {
        init_pair();
        return paths_.size() == pair_->states().size() - 1;
      }

      word_t greedy()
      {
        return synchro(&word_synchronizer::dist);
      }

      word_t cycle()
      {
        return cycle_();
      }

      word_t synchroP()
      {
        return synchro(&word_synchronizer::phi_1);
      }

      word_t synchroPL()
      {
        return synchro(&word_synchronizer::phi_2);
      }

      word_t fastsynchro()
      {
        return fastsynchro_();
      }

    private:
      using heuristic_t = auto (word_synchronizer::*)(state_t) const -> int;

      word_t synchro(heuristic_t heuristic)
      {
        init_synchro();
        while (!todo_.empty())
          {
            int min = std::numeric_limits<int>::max();
            state_t s_min = 0;
            for (auto s : todo_)
              {
                int d = (this->*(heuristic))(s);
                if (d < min)
                  {
                    min = d;
                    s_min = s;
                  }
              }

            apply_path(recompose_path(s_min));
          }
        return res_;
      }

      word_t cycle_()
      {
        init_synchro(true);
        bool first = true;
        state_t previous = 0;
        while (!todo_.empty())
          {
            int min = std::numeric_limits<int>::max();
            state_t s_min = 0;
            for (auto s : todo_)
              {
                state_name_t o = pair_->get_origin(s);
                if (!first && o.first != previous && o.second != previous)
                  continue;
                int d = dist(s);
                if (d < min)
                  {
                    min = d;
                    s_min = s;
                  }
              }

            const auto& path = recompose_path(s_min);
            state_name_t pair_end
              = pair_->get_origin(pair_->dst_of(path[path.size() - 1]));
            assert(pair_end.first == pair_end.second);
            previous = pair_end.first;
            first = false;
            apply_path(path);
          }
        return res_;
      }

      word_t fastsynchro_()
      {
        init_synchro();

        // The drawback of this algorithm is that it does not guarantee us to
        // converge, so we this to counter prevent potential infinite loops.
        unsigned count = 0;
        while (!todo_.empty())
          {
            // compute lmin = arg min { phi_3(l) } forall l in labelset
            label_t lmin;
            int min = std::numeric_limits<int>::max();
            for (const auto& l : pair_->labelset()->generators())
              {
                int cur_min = phi_3(l);
                if (cur_min < min)
                  {
                    min = cur_min;
                    lmin = l;
                  }
              }

            unsigned sq_bound = aut_->states().size();
            if (min < 0 && count++ < (sq_bound * sq_bound))
              apply_label(lmin);
            else
              {
                // fallback on the phi_2 heuristic, with a size restriction.
                int count = 0;
                size_t t = todo_.size();
                int bound = std::min(aut_->states().size(), (t * t - t / 2));
                int min = std::numeric_limits<int>::max();
                state_t s_min = 0;
                for (auto s : todo_)
                  {
                    if (count++ >= bound)
                      break;
                    int d = phi_2(s);
                    if (d < min)
                      {
                        min = d;
                        s_min = s;
                      }
                  }
                apply_path(recompose_path(s_min));
              }
          }
        return res_;
      }

      /// Compute dist(d(s, w)) - dist(s).
      int delta(state_t p, const std::vector<transition_t>& w) const
      {
        state_t np = p;
        for (auto t : w)
          np = dest_state(np, pair_->label_of(t));
        return dist(np) - dist(p);
      }

      /// Heuristic used for SynchroP.
      /// phi_1(p) = Sum delta(s, w) for all s in the active states, with
      /// s != p. w is the shortest word that syncs the pair p.
      int phi_1(state_t p) const
      {
        int res = 0;
        auto w = recompose_path(p);
        for (auto s: todo_)
          if (s != p)
            res += delta(s, w);
        return res;
      }

      /// Heuristic used for SynchroPL.
      /// phi2(p) = phi_1(p).
      int phi_2(state_t p) const
      {
        return phi_1(p) + dist(p);
      }

      /// Heuristic used for FastSynchro.
      /// phi3(l) = Sum dist(d(s, l)) - dist(s) forall s in the active states.
      int phi_3(const label_t& l) const
      {
        int res = 0;
        for (auto s: todo_)
          res += dist(dest_state(s, l)) - dist(s);
        return res;
      }
    };
  }


  /*-----------------------------.
  | is_synchronizing(automaton). |
  `-----------------------------*/

  /// Whether this automaton is synchronizing, i.e., has synchronizing words.
  template <Automaton Aut>
  bool is_synchronizing(const Aut& aut)
  {
    vcsn::detail::word_synchronizer<Aut> sync(aut);
    return sync.is_synchronizing();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_synchronizing(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return vcsn::is_synchronizing(a);
      }
    }
  }


  /*-------------------------------.
  | synchronizing_word(automaton). |
  `-------------------------------*/

  /// Return a synchronizing word for \a aut using algo \a algo.
  template <Automaton Aut>
  word_t_of<Aut>
  synchronizing_word(const Aut& aut, const std::string& algo = "greedy")
  {
    vcsn::detail::word_synchronizer<Aut> sync(aut);
    if (boost::iequals(algo, "greedy") || boost::iequals(algo, "eppstein"))
      return sync.greedy();
    else if (boost::iequals(algo, "cycle"))
      return sync.cycle();
    else if (boost::iequals(algo, "synchrop"))
      return sync.synchroP();
    else if (boost::iequals(algo, "synchropl"))
      return sync.synchroPL();
    else if (boost::iequals(algo, "fastsynchro"))
      return sync.fastsynchro();
    else
      raise("synchronizing_word: invalid algorithm: ", str_escape(algo));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename String>
      label
      synchronizing_word(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        auto word = vcsn::synchronizing_word(a, algo);
        return {make_wordset(*a->labelset()), word};
      }
    }
  }
}
