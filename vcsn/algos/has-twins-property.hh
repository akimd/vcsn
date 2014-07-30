#ifndef VCSN_ALGOS_HAS_TWINS_PROPERTY_HH
# define VCSN_ALGOS_HAS_TWINS_PROPERTY_HH

# include <stack>
# include <vector>
# include <unordered_set>
# include <unordered_map>

# include <vcsn/misc/unordered_set.hh>
# include <vcsn/misc/unordered_map.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/algos/accessible.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  /*----------.
  |  reverse  |
  `----------*/

  /// Inverse the weight of all edges of \a aut.
  template <typename Aut>
  Aut&
  inverse_here(Aut& aut)
  {
    const auto& ws = *aut->weightset();
    for (auto t : aut->all_transitions())
      aut->set_weight(t, ws.rdiv(ws.one(), aut->weight_of(t)));
    return aut;
  }

  template <typename Aut>
  auto
  inverse(const Aut& aut)
    -> decltype(::vcsn::copy(aut))
  {
    auto res = copy(aut);
    return inverse_here(res);
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      automaton
      inverse(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::inverse(a));
      }

      REGISTER_DECLARE(inverse,
                       (const automaton&) -> automaton);
    }
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
    class scc_kosaraju
    {
    public:
      using state_t = state_t_of<Aut>;
      using component_t = std::unordered_set<state_t>;
      using components_t = std::vector<component_t>;

      scc_kosaraju(const Aut& aut)
      {
        auto trans = ::vcsn::transpose(aut);
        auto todo = reverse_postorder(trans);
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

  /// Find all strongly connected components of \a aut.
  template <typename Aut>
  const std::vector<std::unordered_set<state_t_of<Aut>>>
  components(const Aut& aut)
  {
    detail::scc_kosaraju<Aut> scc(aut);
    return scc.components();
  }


  /*-----------------.
  | cycle_identity.  |
  `-----------------*/

  namespace detail
  {
    /// Whether the weight of beetween two states on component,
    /// it is always unique.
    template <typename Aut>
    class cycle_identity_impl
    {
    public:
      using transition_t = transition_t_of<Aut>;
      using weight_t = weight_t_of<Aut>;
      using state_t = state_t_of<Aut>;
      using component_t = std::unordered_set<state_t> ;

      cycle_identity_impl() {}

      // Calcule the weight with depth first search by weight
      // and compare the weight of two state is unique.
      bool check(const component_t& component, const Aut& aut)
      {
        std::unordered_map<state_t, weight_t> wm;
        const auto& ws = *aut->weightset();
        auto s0 = *component.begin();
        wm[s0] = ws.one();

        for (auto t : transitions_by_dfs_(component, aut))
          {
            auto src = aut->src_of(t);
            auto dst = aut->dst_of(t);
            if (!has(wm, dst))
              wm.emplace(dst, ws.mul(wm[src], aut->weight_of(t)));
            if (!ws.equals(wm[dst], ws.mul(wm[src], aut->weight_of(t))))
              return false;
          }
        return true;
      }

    private:
      using transitions_t = std::vector<transition_t>;
      /// Visit all of edges of a component by depth first search.
      transitions_t
      transitions_by_dfs_(const component_t& component,
                          const Aut& aut)
      {
        transitions_t res;
        std::set<state_t> marked;
        std::stack<transition_t> todo;

        auto s0 = *component.begin();
        marked.emplace(s0);
        todo.push(s0);
        while (!todo.empty())
          {
            auto s = todo.top();
            todo.pop();

            for (auto t : aut->out(s))
              {
                auto dst = aut->dst_of(t);
                if (has(component, dst))
                  {
                    if (marked.find(dst) == marked.end())
                      {
                        marked.emplace(dst);
                        todo.push(dst);
                      }
                    res.emplace_back(t);
                  }
              }
          }
        return res;
      }
    };
  }

  /// Check the weight of two states on this component is unique.
  template <typename Aut>
  bool cycle_identity(const std::unordered_set<state_t_of<Aut>>& c,
                      const Aut& aut)
  {
    detail::cycle_identity_impl<Aut> ci;
    return ci.check(c, aut);
  }


  /*---------------------.
  | has_twins_property.  |
  `---------------------*/

  /// Whether \a aut has the twins property.
  template <typename Aut>
  bool has_twins_property(const Aut& aut)
  {
    // TODO: Check cycle-unambiguous.
    auto trim = ::vcsn::trim(aut);
    auto inv = inverse(trim);
    auto a = ::vcsn::product(inv, trim);

    // Find all components of automate a.
    auto cs = components(a);

    // Check unique weight of two states on each component.
    for (auto c : cs)
      if (!cycle_identity(c, a))
        return false;

    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      // Bridge.
      template <typename Aut>
      bool has_twins_property(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::has_twins_property(a);
      }

      REGISTER_DECLARE(has_twins_property,
                       (const automaton&) -> bool);
    }
  }
}
#endif // !VCSN_ALGOS_HAS_TWINS_PROPERTY_HH
