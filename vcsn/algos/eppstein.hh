#pragma once

#include <vector>

#include <vcsn/misc/fibonacci_heap.hh>
#include <vcsn/misc/unordered_map.hh>
#include <vcsn/algos/path.hh>
#include <vcsn/algos/shortest-path-tree.hh>
#include <vcsn/algos/implicit-path.hh>

namespace vcsn
{
  /// Eppstein implementation of the K shortest path problem adapted to Vcsn.
  ///
  /// Based on `Finding the k shortest paths`, David Eppstein (1997).
  template <Automaton Aut>
  class eppstein
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;
    using implicit_path_t = implicit_path<automaton_t>;
    using sidetrack_costs_t = std::unordered_map<transition_t, weight_t>;

  public:
    eppstein(const automaton_t& aut)
      : aut_{aut}
    {}

    using queue_t = vcsn::min_fibonacci_heap<implicit_path_t>;

    /// Compute the \a K shortest paths in the automaton from \a src to \a dst.
    std::vector<path<automaton_t>>
    k_shortest_path(state_t src, state_t dst, int K)
    {
      auto tree = compute_shortest_path_tree(aut_, dst);
      auto sidetrack_edge_costs_map = sidetrack_costs_t();
      auto res = std::vector<path<automaton_t>>{};
      auto queue = queue_t{};
      queue.emplace(aut_, aut_->null_transition(),
                    int(implicit_path_t::null_parent_path),
                    tree.get_weight_of(src));

      for (int k = 0; k < K && !queue.empty(); k++)
      {
        auto k_path_implicit = std::move(queue.top());
        queue.pop();

        auto k_path = k_path_implicit.explicit_path(res, tree, src);
        if (k_path.path_.empty())
          return res;

        res.emplace_back(std::move(k_path));

        add_children_to_queue_(sidetrack_edge_costs_map, src, k_path_implicit, k, queue, tree);
      }

      return res;
    }

  private:
    /// Update queue with children of the first state in the sidetrack path.
    ///
    /// On-the-fly update of the sidetrack costs map (avoid computing every
    /// sidetrack costs when not needed).
    void
    add_children_to_queue_(sidetrack_costs_t& sidetracks, state_t src,
                           const implicit_path_t& k_path_implicit, int k,
                           queue_t& queue, shortest_path_tree<automaton_t>& tree)
    {
      const auto& ws = *aut_->weightset();
      auto k_path_cost = k_path_implicit.weight_;
      auto transition_stack = std::vector<transition_t>{};
      auto s = k == 0 ? src : aut_->dst_of(k_path_implicit.sidetrack_);
      for (auto tr : all_out(aut_, s))
        transition_stack.push_back(tr);

      while (!transition_stack.empty())
      {
        transition_t curr = transition_stack.back();
        transition_stack.pop_back();

        bool has_curr = has(sidetracks, curr);
        if (!has_curr)
        {
          state_t parent = tree.get_parent_of(aut_->src_of(curr));
          if (parent == aut_->null_state() || parent != aut_->dst_of(curr))
          {
            sidetracks[curr] = ws.mul(aut_->weight_of(curr),
                                      ws.rdivide(tree.get_weight_of(aut_->dst_of(curr)),
                                                 tree.get_weight_of(aut_->src_of(curr))));
            has_curr = true;
          }
        }

        if (has_curr)
          queue.emplace(aut_, curr, k, ws.mul(k_path_cost, sidetracks[curr]));
        else
          for (auto tr : all_out(aut_, aut_->dst_of(curr)))
            transition_stack.push_back(tr);
      }
    }

    const automaton_t& aut_;
  };

  /// Compute the \a num lightest paths in the automaton \a aut from \a src to
  /// \a dst. Use Eppstein algorithm.
  template <Automaton Aut>
  std::vector<path<Aut>>
  compute_eppstein(const Aut& aut, state_t_of<Aut> src, state_t_of<Aut> dst, unsigned num)
  {
    auto ksp = eppstein<Aut>(aut);
    return ksp.k_shortest_path(aut->pre(), aut->post(), num);
  }
}
