#pragma once

#include <vcsn/core/automaton.hh>
#include <vcsn/algos/shortest-path-tree.hh>
#include <vcsn/ctx/traits.hh>

namespace vcsn
{
  template <Automaton Aut>
  class implicit_path
  {
    using automaton_t = Aut;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;

  public:
    static constexpr int null_parent_path = -1;

    implicit_path(const automaton_t& aut, transition_t sidetrack,
                  int parent_path, weight_t weight)
    : aut_{aut}
    , sidetrack_{sidetrack}
    , parent_path_{parent_path}
    , weight_{weight}
    {}

    path<automaton_t>
    explicit_path(const std::vector<path<automaton_t>>& ksp,
                  shortest_path_tree<automaton_t>& tree,
                  state_t src)
    {
      auto res = path<automaton_t>(aut_);

      if (parent_path_ != null_parent_path)
        {
          auto& explicit_pref_path = ksp[parent_path_];
          auto& path = explicit_pref_path.path_;

          // The index of the last transition of the path before the sidetrack.
          int last_transition_index = -1;
          for (int i = path.size() - 1; 0 <= i; i--)
            {
              auto curr = path[i];
              if (aut_->dst_of(curr) == aut_->src_of(sidetrack_))
                {
                  last_transition_index = i;
                  break;
                }
            }

          for (unsigned i = 0; i <= last_transition_index; i++)
            res.emplace_back(aut_->weight_of(path[i]), path[i]);

          res.emplace_back(aut_->weight_of(sidetrack_), sidetrack_);
        }

      auto s = parent_path_ == null_parent_path ? src : aut_->dst_of(sidetrack_);
      while (s != tree.root_)
        {
          auto next = tree.get_parent_of(s);
          if (s == aut_->null_state() || next == aut_->null_state())
            return path<automaton_t>(aut_);
          auto weight = tree.get_weight_of(s) - tree.get_weight_of(next);
          auto t = find_transition(s, next);
          assert(t != aut_->null_transition());
          res.emplace_back(weight, t);
          s = next;
        }

      return res;
    }

    transition_t
    find_transition(state_t src, state_t dst) const
    {
      auto min_tr = aut_->null_transition();
      for (auto tr : detail::outin(aut_, src, dst))
        if (min_tr == aut_->null_transition()
            || aut_->weight_of(tr) < aut_->weight_of(min_tr))
          min_tr = tr;
      return min_tr;
    }

    bool operator<(const implicit_path& other) const
    {
      return weight_ < other.weight_;
    }

    // FIXME: private
    const automaton_t& aut_;
    transition_t sidetrack_;
    int parent_path_;
    weight_t weight_;
  };
}
