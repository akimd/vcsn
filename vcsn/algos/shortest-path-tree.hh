#pragma once

#include <vcsn/misc/map.hh>
#include <vcsn/algos/detail/dijkstra-node.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/fibonacci_heap.hh>

namespace vcsn
{
  namespace detail
  {
    /// Shortest Path Tree
    ///
    /// Represent the tree of nodes in the graph with each node's
    /// parent being their lightest predecessor in the automaton
    /// (i.e., the path is from destination to source).
    template <typename Aut>
    class shortest_path_tree
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using node_t = dijkstra_node<automaton_t>;
      using map_t = std::unordered_map<state_t, node_t>;

    public:
      shortest_path_tree(const automaton_t& aut, state_t root)
        : root_{root}
        , aut_{aut}
      {
        compute_();
      }

      void
      add(const node_t& n)
      {
        states_[n.state_] = n;
      }

      void
      set_parent_of(state_t s, state_t parent)
      {
        if (has(states_, s))
          states_[s].set_parent(parent);
        else
          states_[s] = node_t{aut_, s, parent};
      }

      weight_t
      get_weight_of(state_t s) const
      {
        auto it = states_.find(s);
        if (it == states_.end())
          return aut_->weightset()->max();
        else
          return it->second.get_weight();
      }

      node_t&
      get_node_of(state_t s)
      {
        auto it = states_.find(s);
        if (it == states_.end())
        {
          auto elt = node_t{aut_, s, aut_->null_state()};
          auto p = states_.emplace(s, elt);
          it = p.first;
        }
        return it->second;
      }

      state_t
      get_parent_of(state_t s) const
      {
        auto it = states_.find(s);
        if (it == states_.end())
          return aut_->null_state();
        else
          return it->second.get_parent();
      }

      state_t
      get_root() const
      {
        return root_;
      }

      node_t& operator[](state_t s)
      {
        return states_[s];
      }

    private:
      /// Compute the shortest path tree of \a aut_ starting from root_.
      ///
      /// Create a shortest path tree with root_ as root, then
      /// construct the tree by going backwards in the automaton with
      /// a basic shortest path method (heap of incoming nodes sorted
      /// by nodes' weight).
      void
      compute_()
      {
        using queue_t = vcsn::min_fibonacci_heap<node_t>;
        using handle_t = typename queue_t::handle_type;
        auto handles = std::unordered_map<state_t_of<automaton_t>, handle_t>{};

        auto queue = queue_t{};
        auto& src_node = get_node_of(get_root());
        src_node.set_weight(weightset_t_of<automaton_t>::one());
        src_node.set_depth(0);
        handles[src_node.get_state()] = queue.emplace(src_node);

        const auto& ws = *aut_->weightset();
        while (!queue.empty())
        {
          auto current = std::move(queue.top());
          queue.pop();
          auto s = current.get_state();
          // This saves us very little time compared to retrieval at
          // each iteration.
          const auto curr_weight = current.get_weight();
          const auto curr_depth = current.get_depth();

          for (auto t: all_in(aut_, s))
          {
            auto& neighbor = get_node_of(aut_->src_of(t));
            auto dist = neighbor.get_weight();
            // In the case of Nmin, this computation is costly because of the
            // further verification on whether lhs or rhs is max_int but using
            // lhs + rhs would disable genericity.
            auto new_dist = ws.mul(curr_weight, aut_->weight_of(t));
            if (ws.less(new_dist, dist))
            {
              neighbor.set_weight(new_dist);
              neighbor.set_depth(curr_depth + 1);
              neighbor.set_parent(s);
              auto p = handles.emplace(neighbor.get_state(), handle_t{});
              if (p.second)
                p.first->second = queue.emplace(neighbor);
              else
                queue.update(p.first->second);
            }
          }
        }
      }

      map_t states_;
      state_t root_;
      const automaton_t& aut_;
    };

    template <typename Aut>
    shortest_path_tree<Aut>
    make_shortest_path_tree(const Aut& aut, state_t_of<Aut> root)
    {
      return {aut, root};
    }
  }
}
