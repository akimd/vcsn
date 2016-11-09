#pragma once

#include <limits>

#include <boost/optional.hpp>

#include <vcsn/ctx/traits.hh>

namespace vcsn
{
  template <Automaton Aut>
  class dijkstra_node
  {
    using automaton_t = Aut;
    using weight_t = weight_t_of<automaton_t>;
    using state_t = state_t_of<automaton_t>;
  public:
    dijkstra_node() = default;

    dijkstra_node(state_t state, const boost::optional<weight_t>& weight,
                  state_t parent, unsigned depth = std::numeric_limits<unsigned>::max())
      : state_{state}
      , weight_{weight}
      , parent_{parent}
      , depth_{depth}
    {}

    bool
    operator<(const dijkstra_node& other) const
    {
      if (!weight_)
        return true;
      else if (!other.weight_)
        return false;
      else
        return *weight_ < *other.weight_;
    }

    weight_t
    get_weight() const
    {
      // FIXME: max
      return weight_ ? *weight_ : std::numeric_limits<weight_t>::max();
    }

    void
    set_weight(weight_t weight)
    {
      weight_ = weight;
    }

    // FIXME: private
    unsigned depth_;
    state_t state_;
  private:
    boost::optional<weight_t> weight_;
  public:
    state_t parent_;
  };
}
