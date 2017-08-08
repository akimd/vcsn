#pragma once

#include <vcsn/algos/shortest-path-tree.hh>
#include <vcsn/core/automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/algorithm.hh>

namespace vcsn
{
  namespace detail
  {
    /// Implicit Path representation.
    ///
    /// Abstract representation of paths represented by their parent
    /// path (N-th shortest path, previously computed) and the
    /// sidetrack transition (variation of the previous shortet path
    /// with a sidetrack transition).  The path's weight is stored to
    /// be able to sort these paths in the next shortest path
    /// retrieval.
    template <Automaton Aut>
    class implicit_path
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using path_t = path<automaton_t>;

    public:
      static constexpr int null_parent_path = -1;

      implicit_path(const automaton_t& aut, transition_t sidetrack,
                    int parent_path, weight_t weight)
        : aut_{aut}
        , sidetrack_{sidetrack}
        , parent_path_{parent_path}
        , weight_{weight}
      {}

      /// Create the explicit representation of the implicit path.
      ///
      /// Use the vector of previous \a ksp results to retrieve the
      /// parent path.  Find the prefix path of sidetrack in the
      /// parent path and push it in the result path. Then, add
      /// transitions from sidetrack_ to the destination using \a
      /// tree.  In case of initial path, use \a src as the prefix
      /// path.
      path_t
      explicit_path(const std::vector<path_t>& ksp,
                    shortest_path_tree<automaton_t>& tree,
                    state_t src)
      {
        auto res = path_t(aut_);

        if (parent_path_ != null_parent_path)
          {
            auto& explicit_pref_path = ksp[parent_path_];
            const auto& path = explicit_pref_path.get_path();

            // The index of the last transition of the path before the
            // sidetrack. Using the last occurence of the sidetrack is necessary
            // to avoid computing the same path each time in the case of loops.
            // If the sidetrack appears twice and we rebuild our path after the
            // first one then we lost the previous path.
            auto i = std::find_if(path.rbegin(), path.rend(), [&] (auto curr) {
                return aut_->dst_of(curr) == aut_->src_of(sidetrack_);
              });
            assert(i != path.rend());
            // Make it forward iterator, but one iteration further (in
            // the forward direction).
            auto last = i.base();
            for (auto i = path.begin(); i != last; ++i)
              res.emplace_back(aut_->weight_of(*i), *i);
            res.emplace_back(aut_->weight_of(sidetrack_), sidetrack_);
          }

        auto s = parent_path_ == null_parent_path
          ? src : aut_->dst_of(sidetrack_);

        const auto& ws = *aut_->weightset();
        while (s != tree.get_root())
          {
            auto next = tree.get_parent_of(s);
            if (s == aut_->null_state() || next == aut_->null_state())
              return path_t(aut_);
            auto weight = ws.rdivide(tree[s].get_weight(),
                                     tree[next].get_weight());
            auto t = find_transition(s, next);
            assert(t != aut_->null_transition());
            res.emplace_back(weight, t);
            s = next;
          }

        return res;
      }

      /// Find the lightest transition from \a src to \a dst.
      transition_t
      find_transition(state_t src, state_t dst) const
      {
        // GCC 5 and 6 do not capture as const references
        // (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66735).  So
        // instead of `[&aut = *aut_, &ws = *aut_->weightset()]`
        // we capture this.
        return min_forward(detail::outin(aut_, src, dst),
                           [this] (auto t1, auto t2)
                           {
                             return aut_->weightset()->less
                               (aut_->weight_of(t1),
                                aut_->weight_of(t2));
                           });
      }

      bool operator<(const implicit_path& other) const
      {
        return aut_->weightset()->less(weight_, other.weight_);
      }

      const weight_t& get_weight() const
      {
        return weight_;
      }

      transition_t get_sidetrack() const
      {
        return sidetrack_;
      }

    private:
      const automaton_t& aut_;
      transition_t sidetrack_;
      /// Parent path indexes in the results array. `null_parent_path`
      /// if the path has no parent.
      int parent_path_;
      weight_t weight_;
    };
  }
}
