#pragma once

#include <limits>

#include <vcsn/ctx/traits.hh>

namespace vcsn
{
  namespace detail
  {
    /// Dijkstra Node implementation.
    ///
    /// Abstract representation of each node in shortest paths.
    /// Represented by their corresponding state, weight, depth in the
    /// automaton, and parent in the shortest path tree.  Used by
    /// Eppstein algorithm to retrieve the next best predecessor to be
    /// treated. Sorted in a heap considering their weights (during
    /// the first path computation). Default constructed weights
    /// correspond to the maximum value of the weightset.
    template <Automaton Aut>
    class dijkstra_node
    {
      using automaton_t = Aut;
      using self_t = dijkstra_node;
      using weight_t = weight_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
    public:
      dijkstra_node() = default;

      dijkstra_node(const automaton_t& aut, state_t state,
                    state_t parent,
                    weight_t weight = weightset_t::max(),
                    unsigned depth = std::numeric_limits<unsigned>::max())
        : depth_{depth}
        , state_{state}
        , parent_{parent}
        , weight_{weight}
        , ws_{aut->weightset().get()}
      {}

      /// Compare weights, used to order nodes in the shortest path heap.
      bool
      operator<(const self_t& other) const
      {
        return ws_->less(weight_, other.weight_);
      }

      /// If there is no weight in the node then its weight is the weightset's
      /// maximum.
      weight_t
      get_weight() const
      {
        return weight_;
      }

      void
      set_weight(weight_t weight)
      {
        weight_ = weight;
      }

      state_t
      get_state() const
      {
        return state_;
      }

      unsigned
      get_depth() const
      {
        return depth_;
      }

      void
      set_depth(unsigned depth)
      {
        depth_ = depth;
      }

      void
      set_parent(state_t parent)
      {
        parent_ = parent;
      }

      state_t
      get_parent() const
      {
        return parent_;
      }

    private:
      unsigned depth_;
      state_t state_;
      state_t parent_;
      weight_t weight_;
      // We have 2 alternatives to raw pointer:
      // - Reference: Not possible as the dijkstra node is used in a boost
      //   container that requires default contructible objects.
      // - Shared pointer: Using the shared pointer returned by
      //   `aut->weightset()` would work but slows the algorithm down
      //   as a lot of dijkstra nodes are created.  Hence, we stick
      //   with the raw pointer for now.
      const weightset_t *ws_;
    };
  }
}
