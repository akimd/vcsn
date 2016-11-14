#pragma once

#include <vcsn/misc/map.hh>
#include <vcsn/algos/dijkstra-node.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/fibonacci_heap.hh>

namespace vcsn
{
  template <typename Aut>
  class shortest_path_tree
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;
    using dijkstra_node_t = dijkstra_node<automaton_t>;

  public:
    shortest_path_tree(const automaton_t& aut, state_t root)
      : root_{root}
      , aut_{aut}
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
        states_[s] = dijkstra_node_t{aut_, s, {}, parent};
    }

    weight_t
    get_weight_of(state_t s)
    {
      auto it = states_.find(s);
      if (it != states_.end())
        return it->second.get_weight();
      else
        return aut_->weightset()->max();
    }

    dijkstra_node_t&
    get_node_of(state_t s)
    {
      auto it = states_.find(s);
      if (it == states_.end())
      {
        auto elt = dijkstra_node_t{aut_, s, {}, Aut::element_type::null_state()};
        auto p = states_.emplace(s, elt);
        it = p.first;
      }
      return it->second;
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
    const automaton_t& aut_;
  };

  template <typename Aut>
  shortest_path_tree<Aut>
  compute_shortest_path_tree(const Aut& aut, state_t_of<Aut> src)
  {
    using automaton_t = Aut;
    using dijkstra_node_t = dijkstra_node<automaton_t>;
    using queue_t = vcsn::min_fibonacci_heap<dijkstra_node_t>;
    using handle_t = typename queue_t::handle_type;
    auto handles = std::unordered_map<state_t_of<automaton_t>, handle_t>{};

    auto predecessor_tree = shortest_path_tree<automaton_t>(aut, src);
    auto queue = queue_t{};
    auto& src_node = predecessor_tree.get_node_of(predecessor_tree.root_);
    src_node.set_weight(weightset_t_of<automaton_t>::one());
    src_node.depth_ = 0;
    handles[src_node.state_] = queue.emplace(src_node);

    const auto& ws = *aut->weightset();
    while (!queue.empty())
    {
      auto current = std::move(queue.top());
      queue.pop();
      auto s = current.state_;
      // This saves us very little time compared to retrieval at each iteration.
      const auto curr_weight = current.get_weight();
      const auto curr_depth = current.depth_;

      for (auto tr : all_in(aut, s))
      {
        auto& neighbor = predecessor_tree.get_node_of(aut->src_of(tr));
        auto dist = neighbor.get_weight();
        // In the case of Nmin, this computation is costly because of the
        // further verification on whether lhs or rhs is max_int but using
        // lhs + rhs would disable genericity.
        auto new_dist = ws.mul(curr_weight, aut->weight_of(tr));
        if (ws.less(new_dist, dist))
        {
          neighbor.set_weight(new_dist);
          neighbor.depth_ = curr_depth + 1;
          neighbor.parent_ = s;
          auto p = handles.emplace(neighbor.state_, handle_t{});
          if (p.second)
            p.first->second = queue.emplace(neighbor);
          else
            queue.update(p.first->second);
        }
      }
    }
    return predecessor_tree;
  }
}
