#pragma once

#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/misc/map.hh>
#include <vcsn/algos/dijkstra-node.hh>
#include <vcsn/ctx/traits.hh>

namespace vcsn
{
  template <typename Aut>
  class shortest_path_tree
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;
    using dijkstra_node_t = dijkstra_node<automaton_t>;

  public:
    shortest_path_tree(state_t root)
      : root_{root}
    {}

    void
    add(const dijkstra_node_t& n)
    {
      states_[n.state_] = n;
    }

    void
    set_parent_of(state_t s, state_t parent)
    {
      if (has(states_, s))
        states_[s].parent_ = parent;
      else
        states_[s] = dijkstra_node_t{s, {}, parent};
    }

    state_t
    get_parent_of(state_t s)
    {
      auto it = states_.find(s);
      if (it != states_.end())
        return it->second.parent_;
      else
        return automaton_t::element_type::null_state();
    }

    state_t
    get_parent_of(state_t s) const
    {
      auto it = states_.find(s);
      if (it != states_.end())
        return it->second.parent_;
      else
        return automaton_t::element_type::null_state();
    }

  // FIXME: private
    std::unordered_map<state_t, dijkstra_node_t> states_;
    state_t root_;
  };

  template <typename Aut>
  shortest_path_tree<Aut>
  compute_shortest_path_tree(const Aut& aut, state_t_of<Aut> src)
  {
    using automaton_t = Aut;
    using dijkstra_node_t = dijkstra_node<automaton_t>;
    using profile_t = dijkstra_node_t;
    // We want a min-heap.
    using comparator_t = boost::heap::compare<std::greater<profile_t>>;
    using queue_t = boost::heap::fibonacci_heap<profile_t, comparator_t>;

    auto predecessor_tree = shortest_path_tree<automaton_t>(src);
    auto queue = queue_t{};
    for (auto s : aut->all_states())
      predecessor_tree.add(dijkstra_node_t{s, {}, aut->null_state()});
    auto src_node = predecessor_tree.states_[predecessor_tree.root_];
    src_node.set_weight(weightset_t_of<automaton_t>::one());
    src_node.depth_ = 0;
    queue.emplace(src_node);

    while (!queue.empty())
    {
      auto current = std::move(queue.top());
      queue.pop();
      auto s = current.state_;

      for (auto tr : all_in(aut, s))
      {
        auto& neighbor = predecessor_tree.states_[aut->src_of(tr)];
        auto dist = neighbor.get_weight();
        auto new_dist = current.get_weight() + aut->weight_of(tr);
        if (new_dist < dist)
        {
          neighbor.set_weight(new_dist);
          neighbor.depth_ = current.depth_ + 1;
          neighbor.parent_ = s;
          queue.emplace(neighbor);
        }
      }
    }
    return predecessor_tree;
  }
}
