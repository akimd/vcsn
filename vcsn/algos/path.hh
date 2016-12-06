#pragma once

#include <vector>

#include <vcsn/ctx/traits.hh>

namespace vcsn
{
  namespace detail
  {
    /// Explicit path representation.
    ///
    /// Held in the resulting Eppstein vector. Represented by the actual vector
    /// of transitions and the total weight of the path.
    template <Automaton Aut>
    class path
    {
      using automaton_t = Aut;
      using weight_t = weight_t_of<Aut>;
      using transition_t = transition_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using path_t = std::vector<transition_t>;
    public:

      weight_t
      path_sum(const automaton_t& aut, const std::vector<transition_t>& elts)
      {
        weight_t res = weightset_t_of<automaton_t>::one();
        for (auto tr : elts)
          res = ws_.mul(res, aut->weight_of(tr));
        return res;
      }

      path(const automaton_t& aut, const std::vector<transition_t>& elts = {})
        : path_{elts}
        , weight_{path_sum(aut, elts)}
        , aut_{aut}
        , ws_{*aut_->weightset()}
      {}

      void
      push_back(transition_t tr, weight_t weight)
      {
        path_.push_back(tr);
        weight_ = ws_.mul(weight_, weight);
      }

      template <typename... Args>
      void emplace_back(weight_t weight, Args&&... args)
      {
        path_.emplace_back(std::forward<Args>(args)...);
        weight_ = ws_.mul(weight_, weight);
      }

      bool
      operator<(const path& other) const
      {
        return ws_.less(weight_, other.weight_);
      }

      /// Construct a monomial from the path.
      template <typename PolynomialSet>
      auto
      make_monomial(const PolynomialSet& ps) const
        -> typename PolynomialSet::monomial_t
      {
        const auto& pls = *ps.labelset();
        const auto& pws = *ps.weightset();
        const auto& ls = *aut_->labelset();
        auto w = pws.one();
        auto l = pls.one();
        for (auto t : path_)
        {
          w = pws.mul(w, aut_->weight_of(t));
          auto nl = aut_->label_of(t);
          if (!ls.is_special(nl))
            l = pls.mul(l, nl);
        }
        return {l, w};
      }

      const path_t& get_path() const
      {
        return path_;
      }

    private:
      path_t path_;
      weight_t weight_;
      const automaton_t& aut_;
      const weightset_t_of<automaton_t>& ws_;
    };
  }
}
