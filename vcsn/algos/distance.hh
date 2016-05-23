#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <boost/range/algorithm/max_element.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/algorithm.hh> // detail::back
#include <vcsn/misc/deque.hh>
#include <vcsn/misc/pair.hh>
#include <vcsn/misc/queue.hh>
#include <vcsn/weightset/nmin.hh>

namespace vcsn
{
  /// Single source shortest distance.
  /// Find shortest (weighted) path from state \a s0
  /// to all states of automaton \a aut.
  template <Automaton Aut>
  std::vector<weight_t_of<Aut>>
  ss_shortest_distance(const Aut& aut, state_t_of<Aut> s0)
  {
    using weight_t = weight_t_of<Aut>;
    using state_t = state_t_of<Aut>;

    auto ws = *aut->weightset();
    auto d = std::vector<weight_t>(states_size(aut), ws.zero());
    d[s0] = ws.one();
    auto r = d;

    auto todo = std::deque<state_t>{s0};
    while (!todo.empty())
      {
        auto s = todo.front();
        todo.pop_front();
        auto r1 = r[s];
        r[s] = ws.zero();
        for (auto t : all_out(aut, s))
          {
            auto dst = aut->dst_of(t);
            auto w1 = ws.mul(r1, aut->weight_of(t));
            auto w = ws.add(d[dst], w1);
            if (!ws.equal(d[dst], w))
              {
                d[dst] = w;
                r[dst] = ws.add(r[dst], w1);
                if (!has(todo, dst))
                  todo.emplace_back(dst);
              }
          }
    }
    return d;
  }

  /// Find the shortest paths from some states to all the states.
  ///
  /// \param aut    the automaton to traverse.
  /// \param start  the states from which we reach the order states.
  /// \returns a map that, for each state, gives a pair: the shortest distance
  ///          to its parent, and the transition id that allows to reach it.
  template <Automaton Aut>
  std::unordered_map<state_t_of<Aut>,
                     std::pair<unsigned,
                               transition_t_of<Aut>>>
  paths_ibfs(const Aut& aut, const std::vector<state_t_of<Aut>>& start)
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    auto todo = detail::make_queue(start);
    auto marked = std::unordered_set<state_t>{};
    auto parent
      = std::unordered_map<state_t, std::pair<unsigned, transition_t>>{};

    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        for (auto t : all_in(aut, p))
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

  template <Automaton Aut>
  std::vector<std::vector<weight_t_of<Aut>>>
  all_distances(const Aut& aut)
  {
    using automaton_t = Aut;
    using weight_t = weight_t_of<automaton_t>;

    auto ws = aut->weightset();
    auto n = states_size(aut);
    std::vector<std::vector<weight_t>> res(
      n, std::vector<weight_t>(n, ws->zero()));

    for (auto t : all_transitions(aut))
      {
        auto i = aut->src_of(t);
        auto j = aut->dst_of(t);
        res[i][j] = ws->add(res[i][j], aut->weight_of(t));
      }
    for (auto k : aut->states())
      {
        auto reskk = res[k][k] = ws->star(res[k][k]);
        for (auto i : aut->all_states())
          for (auto j : aut->all_states())
            if (i != k && j != k)
              res[i][j] = ws->add(
                res[i][j],
                ws->mul(res[i][k], reskk, res[k][j])
              );
        for (auto i : aut->all_states())
          if (i != k)
            {
              res[k][i] = ws->mul(reskk, res[k][i]);
              res[i][k] = ws->mul(res[i][k], reskk);
            }
      }
    return res;
  }
}
