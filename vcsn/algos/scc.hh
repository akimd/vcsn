#ifndef VCSN_ALGOS_SCC_HH
# define VCSN_ALGOS_SCC_HH

# include <stack>
# include <unordered_map>
# include <unordered_set>
# include <vector>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/misc/unordered_map.hh>
# include <vcsn/misc/unordered_set.hh>

namespace vcsn
{
  /*-------------------------------.
  | strongly connected component.  |
  `-------------------------------*/

  namespace detail
  {
    /// Use Tarjan's algorithm to find all strongly
    /// connected components.
    template <typename Aut>
    class scc_tarjan_impl
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = std::unordered_set<state_t>;
      using components_t = std::vector<component_t>;

      scc_tarjan_impl(const Aut& aut)
      {
        for (auto s : aut->states())
            if (!has(marked_, s))
              dfs(s, aut);
      }

      const components_t components()
      {
        return components_;
      }

    private:
      void dfs(state_t s, const Aut& aut)
      {
        int min = curr_vertex_num_++;
        low_.emplace(s, min);
        marked_.emplace(s);
        stack_.push(s);

        for (auto t : aut->out(s))
          {
            auto dst = aut->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst, aut);
            if (low_[dst] < min)
              min = low_[dst];
          }
        if (min < low_[s])
          {
            low_[s] = min;
            return;
          }

        state_t w;
        components_.emplace_back(component_t{});
        do
          {
            w = stack_.top();
            stack_.pop();
            components_[curr_comp_num_].emplace(w);
            // This vertex belong only one component
            // so remove it by update low value to max size.
            low_[w] = aut->num_states() + 1;
          }
        while (w != s);
        curr_comp_num_++;
      }

      /// The current component number.
      int curr_comp_num_ = 0;
      /// The current visited vertex
      int curr_vertex_num_ = 0;
      /// All compnents
      components_t components_;
      /// List visited vertices
      std::unordered_set<state_t> marked_;
      /// low_[s] is minimum of vertex that it can go
      std::unordered_map<state_t, int> low_;
      /// Contains list vertices same the component
      std::stack<state_t> stack_;
    };
  }


  /*--------------------.
  | reverse_postorder.  |
  `--------------------*/

  namespace detail
  {
    /// Get all vertexs in reverse postorder
    /// by using depth first search.
    template <typename Aut>
    class reverse_postorder_impl
    {
    public:
      using state_t = state_t_of<Aut>;

      reverse_postorder_impl(const Aut& aut)
      {
        for (auto s : aut->all_states())
          if (!has(marked_, s))
            dfs(s, aut);
      }

      std::stack<state_t>& reverse_post()
      {
        return rvp_;
      }

    private:
      void dfs(state_t s, const Aut& aut)
      {
        marked_.emplace(s);
        for (auto t : aut->out(s))
          {
            auto dst = aut->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst, aut);
          }
        rvp_.push(s);
      }
      std::stack<state_t> rvp_;
      std::unordered_set<state_t> marked_;
      std::stack<state_t> todo_;
    };
  }

  /// Get all vertices in reverse postorder.
  template <typename Aut>
  std::stack<state_t_of<Aut>>
  reverse_postorder(const Aut& aut)
  {
    detail::reverse_postorder_impl<Aut> dv(aut);
    return dv.reverse_post();
  }


  /*---------------.
  | scc_kosaraju.  |
  `---------------*/

  namespace detail
  {
    /// Use Kosajaju algorithm for finding all of strongly
    /// connected components.
    template <typename Aut>
    class scc_kosaraju_impl
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = std::unordered_set<state_t>;
      using components_t = std::vector<component_t>;

      scc_kosaraju_impl(const Aut& aut)
      {
        auto trans = ::vcsn::transpose(aut);
        auto todo = ::vcsn::reverse_postorder(trans);
        while (!todo.empty())
          {
            auto s = todo.top();
            todo.pop();
            if (!has(marked_, s))
              {
                dfs(s, aut);
                ++num_;
              }
          }
      }

      const components_t components()
      {
        return components_;
      }

    private:
      void dfs(state_t s, const Aut& aut)
      {
        marked_.emplace(s);
        if (num_ == components_.size())
          components_.emplace_back(component_t{s});
        else
          components_[num_].emplace(s);

        for (auto t : aut->out(s))
          {
            auto dst = aut->dst_of(t);
            if (!has(marked_, dst))
              dfs(dst, aut);
          }
      }

      /// The current component number.
      int num_ = 0;
      components_t components_;
      std::unordered_set<state_t> marked_;
    };
  }

  enum class SCC_ALGO
  {
    TARJAN = 0,
    KOSARAJU = 1
  };

  /// Find all strongly connected components of \a aut.
  template <typename Aut>
  const std::vector<std::unordered_set<state_t_of<Aut>>>
  scc(const Aut& aut, SCC_ALGO algo = SCC_ALGO::TARJAN)
  {
    switch (algo)
      {
      case SCC_ALGO::TARJAN:
        {
          detail::scc_tarjan_impl<Aut> scc(aut);
          return scc.components();
        }
      case SCC_ALGO::KOSARAJU:
        {
          detail::scc_kosaraju_impl<Aut> scc(aut);
          return scc.components();
        }
      }
  }

  /// Get number of strongly connected components.
  template <typename Aut>
  int num_sccs(const Aut& aut)
  {
    return scc(aut).size();
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      int num_sccs(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::num_sccs(a);
      }

      REGISTER_DECLARE(num_sccs,
                       (const automaton&) -> int);
    }
  }

}
#endif // !VCSN_ALGOS_SCC_HH
