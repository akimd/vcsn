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
  std::unordered_map<state_t_of<Aut>,
                     std::pair<unsigned,
                               transition_t_of<Aut>>>
  paths_ibfs(const Aut& aut, std::vector<state_t_of<Aut>> start)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    std::queue<state_t> todo;
    std::unordered_set<state_t> marked;
    std::unordered_map<state_t, std::pair<state_t, transition_t>> parent;

    for (auto s : start)
      todo.push(s);

    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        for (auto t : aut->in(p))
          {
            auto s = aut->src_of(t);
            if (marked.find(s) == marked.end())
              {
                todo.push(s);
                auto cur_p = parent.find(p);
                unsigned cur_d;
                if (cur_p == parent.end())
                    cur_d = 1;
                else
                    cur_d = cur_p->second.first + 1;
                parent[s] = {cur_d, t};
              }
          }
      }
    return parent;
  }

  template<typename Aut>
  std::vector<transition_t_of<Aut>>
  path_bfs(const Aut& aut, state_t_of<Aut> start,
                               state_t_of<Aut> end)
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
          for (auto t : aut->out(p))
            {
              auto s = aut->dst_of(t);
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
