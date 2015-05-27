#pragma once

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <vcsn/algos/copy.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/misc/pair.hh>

namespace vcsn
{
  /// Single source shortest distance.
  /// Find shortest (weighted) path from state \a s0
  /// to all states of automaton \a aut.
  template <typename Aut>
  std::unordered_map<state_t_of<Aut>, weight_t_of<Aut>>
  ss_shortest_distance(const Aut& aut, state_t_of<Aut> s0)
  {
    using weight_t = weight_t_of<Aut>;
    using state_t = state_t_of<Aut>;

    std::unordered_map<state_t, weight_t> d;
    std::unordered_map<state_t, weight_t> r;
    auto ws = *aut->weightset();
    for (auto s : aut->all_states())
      {
        d.emplace(s, ws.zero());
        r.emplace(s, ws.zero());
      }

    std::queue<state_t> todos;
    std::unordered_set<state_t> marked;
    d[s0] = ws.one();
    r[s0] = ws.one();
    todos.emplace(s0);
    marked.emplace(s0);
    while (!todos.empty())
      {
        auto s = todos.front();
        todos.pop();
        auto r1 = r[s];
        r[s] = ws.zero();
        for (auto t : aut->all_out(s))
          {
            auto dst = aut->dst_of(t);
            auto w1 = ws.mul(r1, aut->weight_of(t));
            auto w = ws.add(d[dst], w1);
            if (!ws.equal(d[dst], w))
              {
                d[dst] = w;
                r[dst] = ws.add(r[dst], w1);
                if (!has(marked, dst))
                  {
                    todos.emplace(dst);
                    marked.emplace(dst);
                  }
              }
          }
    }
    d[s0] = ws.one();
    return d;
  }

  /// Find the shortest paths from some states to all the states.
  ///
  /// \param aut    the automaton to traverse.
  /// \param start  the states from which we reach the order states.
  /// \returns a map that, for each state, gives a pair: the shortest distance
  ///          to its parent, and the transition id that allows to reach it.
  template<typename Aut>
  std::unordered_map<state_t_of<Aut>,
                     std::pair<unsigned,
                               transition_t_of<Aut>>>
  paths_ibfs(const Aut& aut, const std::vector<state_t_of<Aut>>& start)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    std::queue<state_t> todo;
    std::unordered_set<state_t> marked;
    std::unordered_map<state_t, std::pair<unsigned, transition_t>> parent;

    for (auto s : start)
      todo.push(s);

    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        for (auto t : aut->all_in(p))
          {
            auto s = aut->src_of(t);
            if (marked.find(s) == marked.end())
              {
                todo.push(s);
                auto cur_p = parent.find(p);
                unsigned cur_d
                  = cur_p == parent.end() ? 1
                  : cur_p->second.first + 1;
                parent[s] = {cur_d, t};
              }
          }
      }
    return parent;
  }

  /// A shortest path between two states.
  ///
  /// \param aut    the automaton
  /// \param start  the starting state
  /// \param end    the target state
  template<typename Aut>
  std::vector<transition_t_of<Aut>>
  path_bfs(const Aut& aut,
           state_t_of<Aut> start, state_t_of<Aut> end)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    std::queue<state_t> todo;
    std::unordered_set<state_t> marked;
    std::unordered_map<state_t, std::pair<state_t, transition_t>> parent;

    todo.push(start);
    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        if (p == end)
          {
            std::vector<transition_t> rpath;
            state_t bt_curr = end;
            while (bt_curr != start)
              {
                transition_t t;
                std::tie(bt_curr, t) = parent.find(bt_curr)->second;
                rpath.push_back(t);
              }
            std::reverse(rpath.begin(), rpath.end());
            return rpath;
          }
        else
          for (auto t : aut->all_out(p))
            {
              auto s = aut->dst_of(t);
              if (marked.find(s) == marked.end())
                {
                  todo.push(s);
                  parent[s] = {p, t};
                }
            }
      }
    // FIXME: why don't we raise here?
    return std::vector<transition_t>();
  }
}
