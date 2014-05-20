#ifndef VCSN_ALGOS_DISTANCE_HH
# define VCSN_ALGOS_DISTANCE_HH

# include <algorithm>
# include <iostream>
# include <queue>
# include <unordered_set>
# include <unordered_map>
# include <vector>

# include <vcsn/algos/copy.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/dyn/label.hh>
# include <vcsn/misc/pair.hh>

namespace vcsn
{
  template<typename Aut>
  std::unordered_map<typename Aut::state_t,
                     std::pair<typename Aut::state_t,
                               typename Aut::transition_t>>
  paths_ibfs(const Aut& aut, typename Aut::state_t start)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;
    using transition_t = typename automaton_t::transition_t;

    std::queue<state_t> todo;
    std::unordered_set<state_t> marked;
    std::unordered_map<state_t, std::pair<state_t, transition_t>> parent;

    todo.push(start);
    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        for (auto t : aut.in(p))
          {
            auto s = aut.src_of(t);
            if (marked.find(s) == marked.end())
              {
                todo.push(s);
                parent[s] = {p, t};
              }
          }
      }
    return parent;
  }

  template<typename Aut>
  std::vector<typename Aut::transition_t>
  path_bfs(const Aut& aut, typename Aut::state_t start,
                               typename Aut::state_t end)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;
    using transition_t = typename automaton_t::transition_t;

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
          for (auto t : aut.out(p))
            {
              auto s = aut.dst_of(t);
              if (marked.find(s) == marked.end())
                {
                  todo.push(s);
                  parent[s] = {p, t};
                }
            }
      }
    return std::vector<transition_t>();
  }
}

#endif // !VCSN_ALGOS_DISTANCE_HH
