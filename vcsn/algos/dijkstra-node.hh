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
    using weightset_ptr = typename automaton_t::element_type::weightset_ptr;
  public:
    dijkstra_node() = default;

    dijkstra_node(const automaton_t& aut, state_t state,
                  boost::optional<weight_t> weight, state_t parent,
                  unsigned depth = std::numeric_limits<unsigned>::max())
      : depth_{depth}
      , state_{state}
      , parent_{parent}
      , weight_{weight}
      , ws_{aut->weightset()}
    {}

    bool
    operator<(const dijkstra_node& other) const
    {
      if (!weight_)
        return true;
      else if (!other.weight_)
        return false;
      else
        return ws_->less(*weight_, *other.weight_);
    }

    weight_t
    get_weight() const
    {
      return weight_ ? *weight_ : ws_->max();
    }

    void
    set_weight(weight_t weight)
    {
      weight_ = weight;
    }

    // FIXME: private
    unsigned depth_;
    state_t state_;
    state_t parent_;
  private:
    boost::optional<weight_t> weight_;
    weightset_ptr ws_;
  };
}
