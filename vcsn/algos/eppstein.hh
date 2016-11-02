#pragma once

#include <vector>

#include <boost/heap/fibonacci_heap.hpp>

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

    using profile_t = implicit_path_t;
    // We want a min-heap.
    using comparator_t = boost::heap::compare<std::greater<profile_t>>;
    using queue_t = boost::heap::fibonacci_heap<profile_t, comparator_t>;

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
                    tree.states_[src].get_weight());

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
    void
    add_children_to_queue_(sidetrack_costs_t& sidetracks, state_t src,
                           const implicit_path_t& k_path_implicit, int k,
                           queue_t& queue, shortest_path_tree<automaton_t>& spt)
    {
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
          state_t parent = spt.get_parent_of(aut_->src_of(curr));
          if (parent == aut_->null_state() || parent != aut_->dst_of(curr))
          {
            sidetracks[curr] = aut_->weight_of(curr)
                             + spt.states_[aut_->dst_of(curr)].get_weight()
                             - spt.states_[aut_->src_of(curr)].get_weight();
            has_curr = true;
          }
        }

        if (has_curr)
          queue.emplace(aut_, curr, k, k_path_cost + sidetracks[curr]);
        else
          for (auto tr : all_out(aut_, aut_->dst_of(curr)))
            transition_stack.push_back(tr);
      }
    }

    const automaton_t& aut_;
  };

  template <Automaton Aut>
  std::enable_if_t<std::is_same<weightset_t_of<Aut>, vcsn::nmin>::value, std::vector<path<Aut>>>
  compute_eppstein(const Aut& aut, state_t_of<Aut> src, state_t_of<Aut> dst, unsigned num)
  {
    auto ksp = eppstein<Aut>(aut);
    return ksp.k_shortest_path(aut->pre(), aut->post(), num);
  }

  template <Automaton Aut>
  std::enable_if_t<!std::is_same<weightset_t_of<Aut>, vcsn::nmin>::value, std::vector<path<Aut>>>
  compute_eppstein(const Aut& aut, state_t_of<Aut> src, state_t_of<Aut> dst, unsigned num)
  {
    raise("eppstein: invalid algorithm with specified weightset: ",
          *aut->weightset(), " is not nmin");
  }
}
